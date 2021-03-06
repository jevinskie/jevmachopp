#pragma once

#include "jevmachopp/Common.h"

#include <atomic>
#include <utility>

// FIXME: find a better place for this, guard with ifdefs maybe
__attribute__((always_inline)) static inline void flush_icache_line(void *addr) {
    asm volatile("dc cvau, %0" ::"r"(addr));
}

template <typename T>
requires requires(T o) {
    requires sizeof(T) == sizeof(uint128_t);
    requires !same_as<T, uint128_t>;
    requires std::is_trivially_copyable_v<T>;
}
class std::atomic<T> : public atomic<uint128_t> {
public:
    using value_type      = T;
    using difference_type = value_type;

public:
    __attribute__((always_inline)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        const uint128_t res = atomic<uint128_t>::load(order);
        return *(value_type *)&res;
    }

    __attribute__((always_inline)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const volatile noexcept;

    __attribute__((always_inline)) operator value_type() const noexcept {
        const uint128_t res = atomic<uint128_t>::load();
        return *(value_type *)&res;
    }

    __attribute__((always_inline)) operator value_type() const volatile noexcept;

    __attribute__((always_inline)) value_type operator=(value_type desired) noexcept {
        return atomic<uint128_t>::operator=(*(uint128_t *)&desired);
    }

    __attribute__((always_inline)) value_type operator=(value_type desired) volatile noexcept;

    __attribute__((always_inline)) void
    store(value_type desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
        atomic<uint128_t>::store(*(uint128_t *)&desired, order);
    }

    __attribute__((always_inline)) void
    store(value_type desired,
          std::memory_order order = std::memory_order_seq_cst) volatile noexcept;
};

namespace std {
using atomic_uint128_t = std::atomic<uint128_t>;
using atomic_int128_t  = std::atomic<int128_t>;
} // namespace std
