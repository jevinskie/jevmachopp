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
        // pid_t *pid_arg_result = NULL;

        // register pid_t *pid_arg __asm("x0") = pid;
        // register const char *path_arg __asm("x1") = path;
        // register const void *attr_buf_arg __asm("x2") = attr_buf;
        // register char *const *argv_arg __asm("x3") = argv;
        // register char *const *envp_arg __asm("x4") = envp;

        // __asm __volatile (
        //       "mov x16, %[syscall_num]     \n\t"
        //       "svc #0x80                   \n\t"
        //     : "=r" (pid_arg)                                     /* outputs */
        //     : [syscall_num] "I" (244), "r" (pid_arg), "r" (path_arg), "r" (attr_buf_arg), "r"
        //     (argv_arg), "r" (envp_arg) /* inputs */
        //                   : "cc", "x5", "x6", "x8", "x9", "x10", "x11", "x12", "x13", "x14",
        //                   "x15", "x16", "x17", "lr" /* clobbers */
        // );
        // pid_arg_result = pid_arg;
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
