#pragma once

#include "jevmachopp/Common.h"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __arm64__
constexpr std::size_t JEV_PAGE_SZ = 0x4000;
#else
#error "Unsupported arch (unknown page size)"
#endif

template <typename T, std::size_t MinNum> class RingBuffer {
public:
    static constexpr auto min_buf_sz = sizeof(T[MinNum]);
    static constexpr auto buf_sz_phys = roundup_pow2_mul(min_buf_sz, JEV_PAGE_SZ);
    static constexpr auto static_size = buf_sz_phys / sizeof(T);
    using ring_buf_t = T[static_size];


    RingBuffer() {
    };

    // private:
    ring_buf_t *m_buf = nullptr;
};
