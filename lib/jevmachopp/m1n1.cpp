#include "jevmachopp/m1n1.h"

#ifdef M1N1

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


extern "C" __attribute__((__noreturn__,__weak__)) void _ZSt24__throw_out_of_range_fmtPKcz(void) {
    assert(!"__throw_out_of_range_fmt stub called");
}

#endif

namespace m1n1 {

constexpr auto MMU_I_CLINE = 6;               // I-cache line size as 1<<MMU_I_CLINE (64)
constexpr auto MMU_CLINE   = 6;               // D-cache line size as 1<<MMU_CLINE (64)
static_assert_cond(MMU_CLINE == MMU_I_CLINE); // lazy, won't handle other cases

void flush_i_and_d_cache(const void *addr, ssize_t size) {
#if defined(__aarch64__) || defined(__arm64__)
    auto aligned_addr = (uintptr_t)addr & ~((1 << MMU_CLINE) - 1);
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
#elif defined(__linux__)
    cacheflush((void *)addr, (int)size, ICACHE | DCACHE);
#else
#error "flush cache not supported"
#endif
}

} // namespace m1n1


__attribute__((naked))
#ifdef JEV_BAREMETAL
__attribute__((noreturn))
#endif
extern "C" void xnu_jump_stub(uint64_t new_bootargs_addr,
                              uint64_t image_addr,
                              uint64_t new_base,
                              uint64_t image_size,
                              uint64_t image_entry) {
#ifndef __aarch64__
#error unsupported arch, only aarc64 supported right now. should stub out or something
#endif
    // clang-format off
    asm volatile(
                // x0: new_bootargs_addr
                // x1: image_addr
                // x2: new_base
                // x3: image_size
                // x4: image_entry
                "1:\n\t"
                "ldp x5, x6, [x1], #16\n\t"
                "stp x5, x6, [x2]\n\t"
                "dc cvau, x2\n\t"
                "ic ivau, x2\n\t"
                "add x2, x2, #16\n\t"
                "sub x3, x3, #16\n\t"
                "cbnz x3, 1b\n\t"

                "mov x1, x4\n\t"
#ifdef JEV_BAREMETAL
                // call entry point with x0 (boot args addr) as only arg
                "br x1\n\t"
#else
                "ret\n\t"
#endif
                ".word 0xdeadbeef"
    );
    // clang-format on
}
