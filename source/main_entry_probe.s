.section .rodata
.balign 4
.Lnce_diag_main_entry_marker:
    .ascii "[NCE-DIAG][BOOT] 44_MAIN_ENTRY\n"
.Lnce_diag_after_runid_marker:
    .ascii "[NCE-DIAG][BOOT] 45_AFTER_RUNID\n"

.text
.balign 4
.global __wrap_main
.type __wrap_main, %function
__wrap_main:
    sub sp, sp, #0x20
    stp x0, x1, [sp]
    str x30, [sp, #0x10]
    adrp x0, .Lnce_diag_main_entry_marker
    add x0, x0, :lo12:.Lnce_diag_main_entry_marker
    mov x1, #31
    bl svcOutputDebugString
    ldr x30, [sp, #0x10]
    ldp x0, x1, [sp]
    add sp, sp, #0x20
    b __real_main
.size __wrap_main, .-__wrap_main

.balign 4
.global __wrap_nce_diag_set_run_id
.type __wrap_nce_diag_set_run_id, %function
__wrap_nce_diag_set_run_id:
    stp x29, x30, [sp, #-0x10]!
    mov x29, sp
    bl __real_nce_diag_set_run_id
    adrp x0, .Lnce_diag_after_runid_marker
    add x0, x0, :lo12:.Lnce_diag_after_runid_marker
    mov x1, #32
    bl svcOutputDebugString
    ldp x29, x30, [sp], #0x10
    ret
.size __wrap_nce_diag_set_run_id, .-__wrap_nce_diag_set_run_id
