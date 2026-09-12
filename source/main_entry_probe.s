.section .rodata
.balign 4
.Lnce_diag_main_entry_marker:
    .ascii "[NCE-DIAG][BOOT] 44_MAIN_ENTRY\n"

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
