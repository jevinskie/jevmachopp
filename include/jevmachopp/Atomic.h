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

#if 1

template <typename T>
requires requires(T o) {
    requires sizeof(T) == sizeof(uint128_t);
    requires !same_as<T, uint128_t>;
}
class std::atomic<T> : public atomic<uint128_t> {
#if 1
public:
    using value_type = T;
    using difference_type = value_type;
    // static constexpr bool is_always_lock_free = true;

#if 0 
public:
    constexpr atomic() noexcept(std::is_nothrow_default_constructible_v<value_type>) : m_pair{} {};
    constexpr atomic(value_type pair) noexcept : m_pair{pair} {};
    atomic(const atomic &) = delete;
#endif

public:
    __attribute__((always_inline, used)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        const uint128_t res = atomic<uint128_t>::load(order);
        return *(value_type *)&res;
    }

    template <std::memory_order order>
    __attribute__((always_inline, used)) value_type load() const noexcept {
        const uint128_t res = atomic<uint128_t>::load(order);
        return *(value_type *)&res;
    }

#if 0
    __attribute__((always_inline)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const volatile noexcept;

#endif

    __attribute__((always_inline, used)) void
    store(value_type desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
        atomic<uint128_t>::store(*(uint128_t *)&desired, order);
    }

#if 0
    __attribute__((always_inline)) void
    store(value_type desired,
          std::memory_order order = std::memory_order_seq_cst) volatile noexcept;

private:
    std::pair<TF, TS> m_pair;
#endif
#endif
};

#endif
