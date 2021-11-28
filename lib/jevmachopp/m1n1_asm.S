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
