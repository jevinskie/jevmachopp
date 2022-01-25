#ifdef JEV_BAREMETAL

#include "jevmachopp/Common.h"

extern "C" {

__attribute__((naked)) int __unw_getcontext(void *ucp) {
    // clang-format off
    asm volatile(
        "stp    x0, x1,  [x0, #0x000]\n\t"
        "stp    x2, x3,  [x0, #0x010]\n\t"
        "stp    x4, x5,  [x0, #0x020]\n\t"
        "stp    x6, x7,  [x0, #0x030]\n\t"
        "stp    x8, x9,  [x0, #0x040]\n\t"
        "stp    x10,x11, [x0, #0x050]\n\t"
        "stp    x12,x13, [x0, #0x060]\n\t"
        "stp    x14,x15, [x0, #0x070]\n\t"
        "stp    x16,x17, [x0, #0x080]\n\t"
        "stp    x18,x19, [x0, #0x090]\n\t"
        "stp    x20,x21, [x0, #0x0A0]\n\t"
        "stp    x22,x23, [x0, #0x0B0]\n\t"
        "stp    x24,x25, [x0, #0x0C0]\n\t"
        "stp    x26,x27, [x0, #0x0D0]\n\t"
        "stp    x28,x29, [x0, #0x0E0]\n\t"
        "str    x30,     [x0, #0x0F0]\n\t"
        "mov    x1,sp\n\t"
        "str    x1,      [x0, #0x0F8]\n\t"
        "str    x30,     [x0, #0x100]\n\t"    // store return address as pc
        // skip cpsr
        "stp    d0, d1,  [x0, #0x110]\n\t"
        "stp    d2, d3,  [x0, #0x120]\n\t"
        "stp    d4, d5,  [x0, #0x130]\n\t"
        "stp    d6, d7,  [x0, #0x140]\n\t"
        "stp    d8, d9,  [x0, #0x150]\n\t"
        "stp    d10,d11, [x0, #0x160]\n\t"
        "stp    d12,d13, [x0, #0x170]\n\t"
        "stp    d14,d15, [x0, #0x180]\n\t"
        "stp    d16,d17, [x0, #0x190]\n\t"
        "stp    d18,d19, [x0, #0x1A0]\n\t"
        "stp    d20,d21, [x0, #0x1B0]\n\t"
        "stp    d22,d23, [x0, #0x1C0]\n\t"
        "stp    d24,d25, [x0, #0x1D0]\n\t"
        "stp    d26,d27, [x0, #0x1E0]\n\t"
        "stp    d28,d29, [x0, #0x1F0]\n\t"
        "str    d30,     [x0, #0x200]\n\t"
        "str    d31,     [x0, #0x208]\n\t"
        "mov    x0, #0\n\t"                   // return UNW_ESUCCESS
        "ret\n\t"
    );
    // clang-format on
}

__attribute__((naked)) void __libunwind_Registers_arm64_jumpto(void *regs) {
    // clang-format off
    asm volatile(
        // skip restore of x0,x1 for now
        "ldp    x2, x3,  [x0, #0x010]\n\t"
        "ldp    x4, x5,  [x0, #0x020]\n\t"
        "ldp    x6, x7,  [x0, #0x030]\n\t"
        "ldp    x8, x9,  [x0, #0x040]\n\t"
        "ldp    x10,x11, [x0, #0x050]\n\t"
        "ldp    x12,x13, [x0, #0x060]\n\t"
        "ldp    x14,x15, [x0, #0x070]\n\t"
        // x16 and x17 were clobbered by the call into the unwinder, so no point in
        // restoring them.
        "ldp    x18,x19, [x0, #0x090]\n\t"
        "ldp    x20,x21, [x0, #0x0A0]\n\t"
        "ldp    x22,x23, [x0, #0x0B0]\n\t"
        "ldp    x24,x25, [x0, #0x0C0]\n\t"
        "ldp    x26,x27, [x0, #0x0D0]\n\t"
        "ldp    x28,x29, [x0, #0x0E0]\n\t"
        "ldr    x30,     [x0, #0x100]\n\t" // restore pc into lr

        "ldp    d0, d1,  [x0, #0x110]\n\t"
        "ldp    d2, d3,  [x0, #0x120]\n\t"
        "ldp    d4, d5,  [x0, #0x130]\n\t"
        "ldp    d6, d7,  [x0, #0x140]\n\t"
        "ldp    d8, d9,  [x0, #0x150]\n\t"
        "ldp    d10,d11, [x0, #0x160]\n\t"
        "ldp    d12,d13, [x0, #0x170]\n\t"
        "ldp    d14,d15, [x0, #0x180]\n\t"
        "ldp    d16,d17, [x0, #0x190]\n\t"
        "ldp    d18,d19, [x0, #0x1A0]\n\t"
        "ldp    d20,d21, [x0, #0x1B0]\n\t"
        "ldp    d22,d23, [x0, #0x1C0]\n\t"
        "ldp    d24,d25, [x0, #0x1D0]\n\t"
        "ldp    d26,d27, [x0, #0x1E0]\n\t"
        "ldp    d28,d29, [x0, #0x1F0]\n\t"
        "ldr    d30,     [x0, #0x200]\n\t"
        "ldr    d31,     [x0, #0x208]\n\t"

        // Finally, restore sp. This must be done after the the last read from the
        // context struct, because it is allocated on the stack, and an exception
        // could clobber the de-allocated portion of the stack after sp has been
        // restored.
        "ldr    x16,     [x0, #0x0F8]\n\t"
        "ldp    x0, x1,  [x0, #0x000]\n\t" // restore x0,x1
        "mov    sp,x16\n\t"                // restore sp
        "ret    x30\n\t"                   // jump to pc
    );
    // clang-format on
}

} // extern "C"

#endif // JEV_BAREMETAL
