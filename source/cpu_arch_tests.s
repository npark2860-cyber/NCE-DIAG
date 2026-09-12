.text
.align 2

.global nce_diag_cpu_nzcv_csel_probe
.type nce_diag_cpu_nzcv_csel_probe, %function
nce_diag_cpu_nzcv_csel_probe:
    // x0=&nzcv_before, x1=&nzcv_after
    mov     x8, x0
    mov     x9, x1
    mov     w16, #5
    mov     w17, #7
    cmp     w16, w17
    mrs     x10, nzcv
    csel    w2, w16, w17, lt
    mrs     x11, nzcv
    str     x10, [x8]
    str     x11, [x9]
    mov     w0, w2
    ret
.size nce_diag_cpu_nzcv_csel_probe, .-nce_diag_cpu_nzcv_csel_probe

.global nce_diag_cpu_reg_preserve_probe
.type nce_diag_cpu_reg_preserve_probe, %function
nce_diag_cpu_reg_preserve_probe:
    // Return a mismatch bitmask for x19..x28 after a direct BL/RET round-trip.
    stp     x29, x30, [sp, #-96]!
    stp     x19, x20, [sp, #16]
    stp     x21, x22, [sp, #32]
    stp     x23, x24, [sp, #48]
    stp     x25, x26, [sp, #64]
    stp     x27, x28, [sp, #80]

    mov     x19, #0x1919
    mov     x20, #0x2020
    mov     x21, #0x2121
    mov     x22, #0x2222
    mov     x23, #0x2323
    mov     x24, #0x2424
    mov     x25, #0x2525
    mov     x26, #0x2626
    mov     x27, #0x2727
    mov     x28, #0x2828

    bl      .Lreg_preserve_leaf

    mov     w0, #0

    mov     x9, #0x1919
    cmp     x19, x9
    cset    w10, ne
    orr     w0, w0, w10

    mov     x9, #0x2020
    cmp     x20, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #1

    mov     x9, #0x2121
    cmp     x21, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #2

    mov     x9, #0x2222
    cmp     x22, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #3

    mov     x9, #0x2323
    cmp     x23, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #4

    mov     x9, #0x2424
    cmp     x24, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #5

    mov     x9, #0x2525
    cmp     x25, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #6

    mov     x9, #0x2626
    cmp     x26, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #7

    mov     x9, #0x2727
    cmp     x27, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #8

    mov     x9, #0x2828
    cmp     x28, x9
    cset    w10, ne
    orr     w0, w0, w10, lsl #9

    ldp     x19, x20, [sp, #16]
    ldp     x21, x22, [sp, #32]
    ldp     x23, x24, [sp, #48]
    ldp     x25, x26, [sp, #64]
    ldp     x27, x28, [sp, #80]
    ldp     x29, x30, [sp], #96
    ret

.Lreg_preserve_leaf:
    nop
    ret
.size nce_diag_cpu_reg_preserve_probe, .-nce_diag_cpu_reg_preserve_probe
