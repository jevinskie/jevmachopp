    .arch armv8-a
    .text

#if defined(__ELF__) && __ELF__
    .global printf
    .align  2
    .type   printf, %function
printf:
    b debug_printf
    .size   printf, .-printf
#endif

#if defined(__ELF__) && __ELF__
#define JMP_STUB_NAME xnu_jump_stub
#define JMP_STUB_END_NAME _xnu_jump_stub_end
#else
#define JMP_STUB_NAME _xnu_jump_stub
#define JMP_STUB_END_NAME __xnu_jump_stub_end
#endif

#define UTRSTAT 0x010
#define UTXH 0x020

// args: new_base + bootargs_off, image_addr, new_base, image_size, image_entry
// x0: new_base + bootargs_off
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
#if defined(__ELF__) && __ELF__
    stp   x0, lr, [sp, #-16]!
    stp   x1, x2, [sp, #-16]!
    mov w0, '@'
    // bl debug_putc
#if 0
    ldr x1, =EARLY_UART_BASE

1:
    ldr w2, [x1, UTRSTAT]
    tst w2, #2
    beq 1b
    str w0, [x1, UTXH]
#endif

    ldp   x1, x2, [sp], #16
    ldp   x0, lr, [sp], #16
#endif

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
    // ret
#else
    ret
#endif
    .pool
#if defined(__ELF__) && __ELF__
    .size   JMP_STUB_NAME, .-JMP_STUB_NAME
#endif
    .global JMP_STUB_END_NAME
JMP_STUB_END_NAME:
    nop
