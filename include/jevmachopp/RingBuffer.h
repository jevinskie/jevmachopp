#pragma once

#include "jevmachopp/Common.h"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

template <typename T, std::size_t Num> class RingBuffer {
public:
    using ring_buf_t = T[2 * Num];
    static constexpr auto buf_sz_phys_v = sizeof(T[Num]);

    RingBuffer() {
        const auto page_sz = getpagesize();
        const auto buf_sz_phys = sizeof(T) * Num;
    };

    // private:
    ring_buf_t *m_buf;
};
