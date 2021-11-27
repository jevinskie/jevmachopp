#include "jevmachopp/m1n1.h"

#if M1N1

extern "C" __attribute__((noreturn)) void
__assert_func(const char *file, int line, const char *function, const char *assertion) {
    __assert_fail(assertion, file, line, function);
}

extern "C" int puts(const char *s) {
    int res = printf("%s\n", s);
    if (res < 0) {
        res = EOF;
    }
    return res;
}

#endif

namespace m1n1 {

constexpr auto MMU_I_CLINE = 6;               // I-cache line size as 1<<MMU_I_CLINE (64)
constexpr auto MMU_CLINE = 6;                 // D-cache line size as 1<<MMU_CLINE (64)
static_assert_cond(MMU_CLINE == MMU_I_CLINE); // lazy, won't handle other cases

void flush_i_and_d_cache(const void *addr, ssize_t size) {
    auto aligned_addr = (uintptr_t)addr & ((1 << MMU_CLINE) - 1);
    size += (uintptr_t)addr - aligned_addr;
    for (; size > 0; size -= (1 << MMU_CLINE), aligned_addr += (1 << MMU_CLINE)) {
        // clang-format off
        asm volatile(
            "ic ivau, %[_addr]\n\t"
            "dc cvau, %[_addr]\n\t"
            : // outputs
            : [_addr] "r"(aligned_addr) // inputs
            : "memory" // clobbers
        );
        // clang-format on
    }
    // clang-format off
    asm volatile(
        "dsb sy\n\t"
        "isb sy"
        : // outputs
        : // inputs
        : "memory" // clobbers
    );
    // clang-format on
}

} // namespace m1n1
