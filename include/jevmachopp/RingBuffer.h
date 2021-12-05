#pragma once

#include "jevmachopp/Common.h"

#include <atomic>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mach/mach.h>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>

#ifdef __arm64__
constexpr std::size_t JEV_PAGE_SZ = 0x4000;
#else
#error "Unsupported arch (unknown page size)"
#endif

template <typename T, std::size_t MinNum> class RingBuffer {
public:
    static constexpr auto MMAP_MAX_TRIES = 16;
    static constexpr auto min_buf_sz_raw = sizeof(T[MinNum]);
    static constexpr auto min_buf_sz = lcm(min_buf_sz_raw, (std::size_t)2);
    static constexpr auto buf_sz_phys = roundup_pow2_mul(min_buf_sz, JEV_PAGE_SZ);
    static constexpr auto static_size = buf_sz_phys / sizeof(T);
    static_assert_cond(is_pow2(static_size));
    static constexpr auto static_size_mask = static_size - 1;

    using value_type = T;
    using pointer = T *;
    using reference = T &;

    template <class... Args> decltype(auto) emplace(Args &&...args) noexcept {
        return *new (&m_buf[(wr_idx_raw++ & static_size_mask)])
            value_type{std::forward<Args>(args)...};
    }

    void push(const T &val) noexcept {
        emplace(val);
    }

    void push(const T &&val) noexcept {
        emplace(std::move(val));
    }

    T &peek() noexcept {
        return m_buf[rd_idx()];
    }

    T pop() noexcept {
        return std::move(m_buf[(rd_idx_raw++ & static_size_mask)]);
    }

    constexpr std::size_t rd_idx() const noexcept {
        return rd_idx_raw & static_size_mask;
    }

    constexpr std::size_t wr_idx() const noexcept {
        return wr_idx_raw & static_size_mask;
    }

    RingBuffer() noexcept : m_buf(nullptr), m_buf_mirror(nullptr), rd_idx_raw(0), wr_idx_raw(0) {
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

    ~RingBuffer() noexcept {
        assert(!munmap(m_buf, buf_sz_phys * 2));
        m_buf = nullptr;
        m_buf_mirror = nullptr;
    }

    // private:
    pointer m_buf;
    pointer m_buf_mirror;
    std::size_t rd_idx_raw;
    std::size_t wr_idx_raw;
};
