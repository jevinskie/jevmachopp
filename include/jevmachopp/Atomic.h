#pragma once

#include "jevmachopp/Common.h"

#include <atomic>
#include <utility>

// FIXME: find a better place for this, guard with ifdefs maybe
__attribute__((always_inline)) static inline void flush_icache_line(void *addr) {
    asm volatile("dc cvau, %0" ::"r"(addr));
}

// struct u64_pair {
//     uint64_t first
// }

template <typename TF, typename TS>
requires requires(TF first, TS second) {
    requires true;
}
class std::atomic<std::pair<TF, TS>> {
public:
    std::pair<TF, TS> pair;
};
