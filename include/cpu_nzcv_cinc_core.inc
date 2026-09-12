.macro NCE_DIAG_CPU_NZCV_CINC_CORE base_reg, before_reg, after_reg, result_reg
    cmp     w16, #0
    mrs     \before_reg, nzcv
    stp     w17, w18, [\base_reg, #0x0c]
    mrs     \after_reg, nzcv
    mov     \result_reg, #2
    cinc    \result_reg, \result_reg, ne
.endm
