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
    requires sizeof(TF) == sizeof(uint64_t);
    requires sizeof(TS) == sizeof(uint64_t);
    requires sizeof(std::pair<TF, TS>) == 2 * sizeof(uint64_t);
}
class std::atomic<std::pair<TF, TS>> {
public:
    using value_type = std::pair<TF, TS>;
    using difference_type = value_type;
    static constexpr bool is_always_lock_free = true;

public:
    constexpr atomic() noexcept(std::is_nothrow_default_constructible_v<value_type>) : m_pair{} {};
    constexpr atomic(value_type pair) noexcept : m_pair{pair} {};
    atomic(const atomic &) = delete;

public:
    __attribute__((always_inline)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        _Atomic unsigned __int128 *aptr = (_Atomic unsigned __int128 *)&m_pair;
        const auto res_raw = __c11_atomic_load(aptr, to_underlying_int(order));
        return *(value_type *)&res_raw;
    }

    __attribute__((always_inline)) value_type
    load(std::memory_order order = std::memory_order_seq_cst) const volatile noexcept;

    __attribute__((always_inline)) void
    store(value_type desired, std::memory_order order = std::memory_order_seq_cst) noexcept;

    __attribute__((always_inline)) void
    store(value_type desired,
          std::memory_order order = std::memory_order_seq_cst) volatile noexcept;

private:
    std::pair<TF, TS> m_pair;
};
