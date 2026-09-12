.section .rodata
.balign 4
.Lsvfprintf_enter_marker:
    .ascii "[NCE-DIAG][BOOT] 56_SVFPRINTF_ENTER\n"
.Lsvfprintf_return_marker:
    .ascii "[NCE-DIAG][BOOT] 57_SVFPRINTF_RETURN\n"

.text
.balign 4
.global __wrap__svfprintf_r
.type __wrap__svfprintf_r, %function
.extern g_svfprintf_probe_active
__wrap__svfprintf_r:
    sub sp, sp, #0x40
    stp x0, x1, [sp, #0x00]
    stp x2, x3, [sp, #0x10]
    str x30, [sp, #0x20]

    adrp x0, .Lsvfprintf_enter_marker
    add x0, x0, :lo12:.Lsvfprintf_enter_marker
    mov x1, #36
    bl svcOutputDebugString

    adrp x9, g_svfprintf_probe_active
    add x9, x9, :lo12:g_svfprintf_probe_active
    mov w10, #1
    str w10, [x9]

    ldp x0, x1, [sp, #0x00]
    ldp x2, x3, [sp, #0x10]
    bl __real__svfprintf_r
    str x0, [sp, #0x28]

    adrp x9, g_svfprintf_probe_active
    add x9, x9, :lo12:g_svfprintf_probe_active
    str wzr, [x9]

    adrp x0, .Lsvfprintf_return_marker
    add x0, x0, :lo12:.Lsvfprintf_return_marker
    mov x1, #37
    bl svcOutputDebugString

    ldr x0, [sp, #0x28]
    ldr x30, [sp, #0x20]
    add sp, sp, #0x40
    ret
.size __wrap__svfprintf_r, .-__wrap__svfprintf_r
