.text
.balign 4
.global nce_diag_emit_debug_witness
.type nce_diag_emit_debug_witness, %function
nce_diag_emit_debug_witness:
    stp x19, x20, [sp, #-0x20]!
    str x30, [sp, #0x10]
    cbz x2, 1f
    mov x19, x2
    mov x20, x3
1:
    bl svcOutputDebugString
    ldr x30, [sp, #0x10]
    ldp x19, x20, [sp], #0x20
    ret
.size nce_diag_emit_debug_witness, .-nce_diag_emit_debug_witness
