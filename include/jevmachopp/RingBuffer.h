#pragma once

#include "jevmachopp/Atomic.h"
#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <future>
#ifdef __APPLE__
#include <mach/mach.h>
#endif
#include <optional>
#include <sys/mman.h>
#include <type_traits>
#include <unistd.h>
#include <utility>

#if defined(__arm64__)
constexpr std::size_t JEV_PAGE_SZ = 0x4000;
#elif defined(__aarch64__) || defined(__x86_64__)
constexpr std::size_t JEV_PAGE_SZ = 0x1000;
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
    static constexpr auto static_size_raw = buf_sz_phys / sizeof(T);
    static constexpr auto static_size = static_size_raw - 1;
    static_assert_cond(is_pow2(static_size_raw));
    static constexpr auto idx_mask = static_size_raw * 2 - 1;
    static constexpr bool is_atomic_pair = MultiProd || MultiCons;

    using nonatomic_idx_t = std::size_t;
    using nonatomic_idx_pair_t = con_pair<nonatomic_idx_t, nonatomic_idx_t>;
    using atomic_idx_t = std::atomic<nonatomic_idx_t>;
    using atomic_idx_pair_t = std::atomic<nonatomic_idx_pair_t>;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    using rd_idx_t = typename std::conditional<MultiCons, atomic_idx_t, nonatomic_idx_t>::type;
    using wr_idx_t = typename std::conditional<MultiProd, atomic_idx_t, nonatomic_idx_t>::type;
    using idx_pair_t =
        typename std::conditional<is_atomic_pair, atomic_idx_pair_t, nonatomic_idx_pair_t>::type;

    template <class... Args>
    decltype(auto) emplace(Args &&...args) noexcept requires(!is_atomic_pair) {
        assert(!full());
        return *new (&m_buf[(wr_idx_raw++ & idx_mask)]) value_type{std::forward<Args>(args)...};
    }

    template <class... Args>
    decltype(auto) emplace(Args &&...args) noexcept requires(is_atomic_pair && !MultiProd) {
        std::size_t wr_raw;
        std::size_t new_idx;
        pointer res_ptr;

        while (true) {
            const nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
            wr_raw = idx_pair_raw.second;
            const auto rd_raw = idx_pair_raw.first;
            const auto rd_full_val = wr_raw - static_size_raw + 1;
            if (rd_raw == rd_full_val) {
                // full, try again
                continue;
            }
            new_idx = wr_raw & idx_mask;
            auto new_ptr = &m_buf[new_idx];
            res_ptr = new (new_ptr) value_type{std::forward<Args>(args)...};
            const auto res = *res_ptr;
            wr_idx_raw = wr_raw + 1;
            return res;
        }
    }

    template <class... Args>
    decltype(auto) emplace(Args &&...args) noexcept requires(is_atomic_pair &&MultiProd) {
        std::size_t wr_raw;
        std::size_t new_idx;
        std::size_t new_wr_raw;
        pointer res_ptr;

        do {
        get_pair:
            nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
            wr_raw = idx_pair_raw.second;
            auto rd_raw = idx_pair_raw.first;
            assert(rd_raw <= wr_raw);
            auto rd_full_val = wr_raw - static_size_raw + 1;
            if (rd_raw == rd_full_val) {
                // full, try again
                goto get_pair;
            }
            new_idx = wr_raw & idx_mask;
            auto new_ptr = &m_buf[new_idx];
            res_ptr = new (new_ptr) value_type{std::forward<Args>(args)...};
            new_wr_raw = wr_raw + 1;
        } while (!wr_idx_raw.compare_exchange_strong(wr_raw, new_wr_raw));

        return *res_ptr;
    }

    void push(const T &val) noexcept {
        emplace(val);
    }

    void push(const T &&val) noexcept {
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

    // totally racey
    std::optional<pointer> peek() noexcept {
        if (empty()) {
            return {};
        }
        return &m_buf[rd_idx()];
    }

    std::optional<value_type> pop() noexcept requires(!MultiCons) {
        if (empty()) {
            return {};
        }
        return std::move(m_buf[(rd_idx_raw++ & idx_mask)]);
    }

    std::optional<value_type> pop() noexcept requires requires() {
        requires MultiCons && std::is_trivially_copyable_v<value_type>;
    }
    {
        T res;

        std::size_t rd_raw;
        std::size_t idx;
        std::size_t new_rd_raw;

        do {
            const nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
            rd_raw = idx_pair_raw.first;
            const auto wr_raw = idx_pair_raw.second;
            assert(rd_raw <= wr_raw);
            if (rd_raw == wr_raw) {
                // empty condition
                return {};
            }
            idx = rd_raw & idx_mask;
            res = m_buf[idx];
            new_rd_raw = rd_raw + 1;
        } while (!rd_idx_raw.compare_exchange_strong(rd_raw, new_rd_raw));

        return res;
    }

#if 0
    std::optional<value_type> pop() noexcept requires requires() {
        requires MultiCons && !std::is_trivially_copyable_v<value_type>;
    }
    {
        T res;

        std::size_t idx_raw;
        std::size_t idx;
        std::size_t new_idx_raw;
        bool cas_res;

        if (empty()) {
            return {};
        }

        do {
            idx_raw = rd_idx_raw;
            idx = idx_raw & idx_mask;
            res = m_buf[idx];
            new_idx_raw = idx_raw + 1;
            cas_res = rd_idx_raw.compare_exchange_strong(idx_raw, new_idx_raw);
        } while (!cas_res);

        return res;
    }
#endif

    constexpr std::size_t rd_idx() const noexcept {
        return rd_idx_raw & idx_mask;
    }

    constexpr std::size_t wr_idx() const noexcept {
        return wr_idx_raw & idx_mask;
    }

    constexpr std::size_t size() const noexcept requires(!is_atomic_pair) {
        const std::size_t rd = rd_idx_raw;
        const std::size_t wr = wr_idx_raw;
        return wr - rd;
    }

    constexpr std::size_t size() const noexcept requires(is_atomic_pair) {
        const nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
        return idx_pair_raw.second - idx_pair_raw.first;
    }

    constexpr std::size_t remaining() const noexcept {
        return static_size - size();
    }

    constexpr bool full() const noexcept requires(!is_atomic_pair) {
        // read write pointer first since nobody else will be changing it
        const std::size_t wr = wr_idx_raw;
        const auto rd_full_val = wr - static_size_raw + 1;
        // load read pointer that others may be updating last
        const std::size_t rd = rd_idx_raw;
        assert(rd <= wr);
        return rd == rd_full_val;
    }

    constexpr bool full() const noexcept requires(is_atomic_pair) {
        const nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
        const std::size_t rd = idx_pair_raw.first;
        const std::size_t wr = idx_pair_raw.second;
        assert(rd <= wr);
        const auto rd_full_val = wr - static_size_raw + 1;
        return rd == rd_full_val;
    }

    constexpr bool empty() const noexcept requires(!is_atomic_pair) {
        // read read pointer first since nobody else will be changing it
        const std::size_t rd = rd_idx_raw;
        // load write pointer that others may be updating last
        const std::size_t wr = wr_idx_raw;
        assert(rd <= wr);
        return rd == wr;
    }

    const bool empty() const noexcept requires(is_atomic_pair) {
        const nonatomic_idx_pair_t idx_pair_raw = ((atomic_idx_pair_t *)&rd_idx_raw)->load();
        assert(idx_pair_raw.first <= idx_pair_raw.second);
        return idx_pair_raw.first == idx_pair_raw.second;
    }

    constexpr bool is_done() const noexcept {
        return done;
    }

    RingBufferBase() noexcept
        : m_buf(nullptr), m_buf_mirror(nullptr), rd_idx_raw(0), wr_idx_raw(0), done(false) {
        for (int try_num = 0; try_num < MMAP_MAX_TRIES; ++try_num) {
            m_buf = (T *)mmap(nullptr, buf_sz_phys * 2, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
            if (!m_buf) {
                continue;
            }
            m_buf_mirror = m_buf + static_size_raw;

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
#if defined(__APPLE__)
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
#else
            const auto vm_remap_res =
                mremap((void *)m_buf, buf_sz_phys, buf_sz_phys, PROT_READ | PROT_WRITE,
                       MREMAP_FIXED, (void *)m_buf_mirror);
            if (vm_remap_res != m_buf_mirror) {
                assert(!munmap(m_buf, buf_sz_phys));
                m_buf = nullptr;
                m_buf_mirror = nullptr;
            }
#endif
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
    void finish() {
        assert(!is_done());
        done = true;
        // flush_icache_line(&done);
    }

    void clear() noexcept {
        rd_idx_raw = 0;
        wr_idx_raw = 0;
        std::memset(m_buf, 0, buf_sz_phys);
        done = false;
    }

    // private:
    pointer m_buf;
    pointer m_buf_mirror;
    // smth_t rd_smth;
    rd_idx_t rd_idx_raw;
    wr_idx_t wr_idx_raw;
    // smth_t wr_smth;
    bool done;
};

template <typename T, std::size_t MinNum>
class RingBuffer : public RingBufferBase<T, MinNum, false, false> {};

template <typename T, std::size_t MinNum>
class MultiProdRingBuffer : public RingBufferBase<T, MinNum, true, false> {};

template <typename T, std::size_t MinNum>
class MultiConsRingBuffer : public RingBufferBase<T, MinNum, false, true> {};

template <typename T, std::size_t MinNum>
class MultiProdConsRingBuffer : public RingBufferBase<T, MinNum, true, true> {};
