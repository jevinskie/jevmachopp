    .arch armv8-a
    .text

    .global printf
    .type   printf, %function
printf:
    b debug_printf
    .size   printf, .-printf

    .global xnu_jump_stub
    .type   xnu_jump_stub, %function
xnu_jump_stub:
    ldp x4, x5, [x1], #8
    stp x4, x5, [x2]
    dc cvau, x2
    ic ivau, x2
    add x2, x2, #8
    sub x3, x3, #8
    cbnz x3, xnu_jump_stub

    mov x1, x6
    br x1
    .size   xnu_jump_stub, .-xnu_jump_stub
