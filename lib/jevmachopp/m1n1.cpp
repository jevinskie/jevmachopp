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


#if 0

#if defined(__ELF__) && __ELF__
#define JMP_STUB_NAME xnu_jump_stub
#define JMP_STUB_END_NAME _xnu_jump_stub_end
#else
#define JMP_STUB_NAME _xnu_jump_stub
#define JMP_STUB_END_NAME __xnu_jump_stub_end
#endif

// x0: new_bootargs_addr
// x1: image_addr
// x2: new_base
// x3: image_size
// x4: image_entry
    .global JMP_STUB_NAME
    .align  2
#if defined(__ELF__) && __ELF__
    .type   JMP_STUB_NAME, %function
#endif
JMP_STUB_NAME:
    ldp x5, x6, [x1], #16
    stp x5, x6, [x2]
    dc cvau, x2
    ic ivau, x2
    add x2, x2, #16
    sub x3, x3, #16
    cbnz x3, JMP_STUB_NAME

    mov x1, x4
#if defined(__ELF__) && __ELF__
    br x1
#else
    ret
#endif
#if defined(__ELF__) && __ELF__
    .size   JMP_STUB_NAME, .-JMP_STUB_NAME
#endif
    .global JMP_STUB_END_NAME
JMP_STUB_END_NAME:
    nop
#endif

__attribute__((naked))
#ifdef JEV_BAREMETAL
__attribute__((noreturn))
#endif
extern "C" void xnu_jump_stub(uint64_t new_bootargs_addr,
                              uint64_t image_addr,
                              uint64_t new_base,
                              uint64_t image_size,
                              uint64_t image_entry) {
// clang-format off
    asm volatile(
                "ldp x5, x6, [x1], #16\n\t"
                "stp x5, x6, [x2]\n\t"
                "dc cvau, x2\n\t"
                "ic ivau, x2\n\t"
                "add x2, x2, #16\n\t"
                "sub x3, x3, #16\n\t"
                "cbnz x3, JMP_STUB_NAME\n\t"

                "mov x1, x4\n\t"
#ifdef JEV_BAREMETAL
                "br x1\n\t"
#else
                "ret\n\t"
#endif
    );
// clang-format on
}
