    .arch armv8-a
    .text

    .global printf
    .type   printf, %function
printf:
    b debug_printf
    .size   printf, .-printf

// args: new_base + bootargs_off, image_addr, new_base, image_size, image_entry
// x0: new_base + bootargs_off
// x1: image_addr
// x2: new_base
// x3: image_size
// x4: image_entry
    .global xnu_jump_stub
    .type   xnu_jump_stub, %function
xnu_jump_stub:
    ldp x5, x6, [x1], #8
    stp x5, x6, [x2]
    dc cvau, x2
    ic ivau, x2
    add x2, x2, #8
    sub x3, x3, #8
    cbnz x3, xnu_jump_stub

    mov x1, x4
    br x1
    .size   xnu_jump_stub, .-xnu_jump_stub
