#pragma once

#include "jevmachopp/Common.h"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mach/mach.h>
#include <sys/mman.h>
#include <type_traits>
#include <unistd.h>
#include <utility>

#include <semaphore.hpp>

#ifdef __arm64__
constexpr std::size_t JEV_PAGE_SZ = 0x4000;
#else
#error "Unsupported arch (unknown page size)"
#endif

template <typename T, std::size_t MinNum, bool MultiProd, bool MultiCons>
requires requires() {
    requires std::is_trivially_destructible_v<T>;
}
class RingBufferBase {
public:
    static constexpr auto MMAP_MAX_TRIES = 16;
    static constexpr auto min_buf_sz_raw = sizeof(T[MinNum]);
    static constexpr auto min_buf_sz = lcm(min_buf_sz_raw, (std::size_t)2);
    static constexpr auto buf_sz_phys = roundup_pow2_mul(min_buf_sz, JEV_PAGE_SZ);
    static constexpr auto static_size = buf_sz_phys / sizeof(T);
    static_assert_cond(is_pow2(static_size));
    static constexpr auto idx_mask = static_size * 2 - 1;

    using atomic_idx_t = std::atomic_size_t;
    using nonatomic_idx_t = std::size_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using rd_idx_t = typename std::conditional<MultiCons, atomic_idx_t, nonatomic_idx_t>::type;
    using wr_idx_t = typename std::conditional<MultiProd, atomic_idx_t, nonatomic_idx_t>::type;

    template <class... Args> decltype(auto) emplace(Args &&...args) noexcept {
        assert(!full());
        return *new (&m_buf[(wr_idx_raw++ & idx_mask)]) value_type{std::forward<Args>(args)...};
    }

    void push(const T &val) noexcept {
        while (full())
            ;
        emplace(val);
    }

    void push(const T &&val) noexcept {
        while (full())
            ;
        emplace(std::move(val));
    }

    void
    emplace_multiple(const std::span<T> &span) requires std::is_trivially_copyable_v<value_type> {
        assert(span.size() <= remaining());
        std::memcpy(&m_buf[wr_idx()], span.data(), span.size_bytes());
        wr_idx_raw += span.size();
    }

    void
    emplace_multiple(const std::span<T> &span) requires(!std::is_trivially_copyable_v<value_type>) {
        assert(span.size() <= remaining());
        std::copy(span.cbegin(), span.cend(), &m_buf[wr_idx()]);
        wr_idx_raw += span.size();
    }

    reference peek() noexcept {
        while (empty())
            ;
        return m_buf[rd_idx()];
    }

    value_type pop(std::binary_semaphore *ready_sem = nullptr,
                   std::binary_semaphore *go_sem = nullptr) noexcept requires(!MultiCons) {
        while (empty())
            ;
        return std::move(m_buf[(rd_idx_raw++ & idx_mask)]);
    }

    value_type pop(std::binary_semaphore *ready_sem = nullptr,
                   std::binary_semaphore *go_sem = nullptr) noexcept requires requires() {
        requires MultiCons && std::is_trivially_copyable_v<value_type>;
    }
    {
        T res;

        std::size_t idx_raw;
        std::size_t idx;
        std::size_t new_idx_raw;
        pointer our_obj;

        do {
            while (empty())
                ;
            idx_raw = rd_idx_raw;
            idx = idx_raw & idx_mask;
            our_obj = &m_buf[idx];
            res = m_buf[idx];
            new_idx_raw = rd_idx_raw + 1;
        } while (!rd_idx_raw.compare_exchange_strong(idx_raw, new_idx_raw));

        our_obj->~T();

        return res;
    }

    value_type pop(std::binary_semaphore *ready_sem = nullptr,
                   std::binary_semaphore *go_sem = nullptr) noexcept requires requires() {
        requires MultiCons && !std::is_trivially_copyable_v<value_type>;
    }
    {
        T res;

        fprintf(stderr, "poppin' ready_sem: %p go_sem: %p\n", ready_sem, go_sem);

        std::size_t idx_raw;
        std::size_t idx;
        std::size_t new_idx_raw;
        pointer our_obj;
        bool cas_res;

        do {
            if (ready_sem) {
                fprintf(stderr, "waiting for ready release\n");
                ready_sem->acquire();
            }
            while (empty())
                ;
            idx_raw = rd_idx_raw;
            idx = idx_raw & idx_mask;
            our_obj = &m_buf[idx];
            if (go_sem) {
                fprintf(stderr, "waiting for go acquire\n");
                go_sem->acquire();
            }
            res = m_buf[idx];
            new_idx_raw = rd_idx_raw + 1;
            cas_res = rd_idx_raw.compare_exchange_strong(idx_raw, new_idx_raw);
            fprintf(stderr, "cas_res: %s\n", YESNOCStr(cas_res));
        } while (!cas_res);

        fprintf(stderr, "destroying %p\n", static_cast<void *>(our_obj));
        our_obj->~T();

        return res;
    }

    constexpr std::size_t rd_idx() const noexcept {
        return rd_idx_raw & idx_mask;
    }

    constexpr std::size_t wr_idx() const noexcept {
        return wr_idx_raw & idx_mask;
    }

    constexpr std::size_t size() const noexcept {
        // FIXME: the below is quite likely wrong
        // we can be conservative and report bigger even if another thread is currently reading
        // so read the read pointer first
        const std::size_t rd = rd_idx_raw;
        const std::size_t wr = wr_idx_raw;
        return wr - rd;
    }

    constexpr std::size_t remaining() const noexcept {
        return static_size - size();
    }

    constexpr bool full() const noexcept requires(!MultiProd) {
        // read write pointer first since nobody else will be changing it
        const std::size_t wr = wr_idx_raw;
        const auto rd_full_val = wr - static_size + 1;
        // load read pointer that others may be updating last
        const std::size_t rd = rd_idx_raw;
        return rd == rd_full_val;
    }

    constexpr bool full() const noexcept requires(MultiProd) {
        // conservative/safe return value for writer
        // report full if another thread completes a write before we return
        std::size_t wr;
        bool res;
        // loop until we check full and the write pointer is still the same
        do {
            wr = wr_idx_raw.load();
            const auto rd_full_val = wr - static_size + 1;
            const std::size_t rd = rd_idx_raw;
            res = rd == rd_full_val;
        } while (wr != wr_idx_raw.load());
        return res;
    }

    constexpr bool empty() const noexcept requires(!MultiCons) {
        // read read pointer first since nobody else will be changing it
        const std::size_t rd = rd_idx_raw;
        // load write pointer that others may be updating last
        const std::size_t wr = wr_idx_raw;
        return rd == wr;
    }

    constexpr bool empty() const noexcept requires(MultiCons) {
        // conservative/safe return value for reader
        // don't report empty if another thread completes a write before we return
        std::size_t rd;
        std::size_t wr;
        bool res;
        // loop until we check full and the write pointer is still the same
        do {
            rd = rd_idx_raw.load();
            wr = wr_idx_raw;
            res = rd == wr;
        } while (rd != rd_idx_raw.load());
        return res;
    }

    RingBufferBase() noexcept
        : m_buf(nullptr), m_buf_mirror(nullptr), rd_idx_raw(0), wr_idx_raw(0) {
        for (int try_num = 0; try_num < MMAP_MAX_TRIES; ++try_num) {
            m_buf = (T *)mmap(nullptr, buf_sz_phys * 2, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
            if (!m_buf) {
                continue;
            }
            m_buf_mirror = m_buf + static_size;

            if (const auto munmap_res = munmap((void *)m_buf_mirror, buf_sz_phys)) {
                printf("munmap_res: %d errno: %d err: %s\n", munmap_res, errno, strerror(errno));
                assert(!munmap(m_buf, buf_sz_phys * 2));
                m_buf = nullptr;
                m_buf_mirror = nullptr;
                continue;
            }
            // err nope thats linux
            // m_buf_mirror = mremap((void*)m_buf, buf_sz_phys, buf_sz_phys, PROT_READ | PROT_WRITE,
            // )

            vm_prot_t cur_prot;
            vm_prot_t max_prot;
            const auto vm_remap_res =
                vm_remap(mach_task_self() /* target_task */,
                         (vm_address_t *)&m_buf_mirror /* target_address */, buf_sz_phys /* size */,
                         0 /* mask */, VM_FLAGS_FIXED /* flags */, mach_task_self(), /* src_task */
                         (vm_address_t)m_buf, /* src_address */
                         false,               /* copy */
                         &cur_prot, &max_prot, VM_INHERIT_COPY);

            if (vm_remap_res != KERN_SUCCESS) {
                assert(!munmap(m_buf, buf_sz_phys));
                m_buf = nullptr;
                m_buf_mirror = nullptr;
            }
            break;
        }
        assert(m_buf && m_buf_mirror);
    };

    ~RingBufferBase() noexcept {
        assert(!munmap(m_buf, buf_sz_phys * 2));
        m_buf = nullptr;
        m_buf_mirror = nullptr;
    }

    // private:
    pointer m_buf;
    pointer m_buf_mirror;
    rd_idx_t rd_idx_raw;
    wr_idx_t wr_idx_raw;
};

template <typename T, std::size_t MinNum>
class RingBuffer : public RingBufferBase<T, MinNum, false, false> {};

template <typename T, std::size_t MinNum>
class MultiProdRingBuffer : public RingBufferBase<T, MinNum, true, false> {};

template <typename T, std::size_t MinNum>
class MultiConsRingBuffer : public RingBufferBase<T, MinNum, false, true> {};

template <typename T, std::size_t MinNum>
class MultiProdConsRingBuffer : public RingBufferBase<T, MinNum, true, true> {};
