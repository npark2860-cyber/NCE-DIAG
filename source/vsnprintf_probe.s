.section .rodata
.balign 4
.Lbefore_marker:
    .ascii "[NCE-DIAG][BOOT] 46_BEFORE_VSNPRINTF\n"
.Lgetreent_marker:
    .ascii "[NCE-DIAG][BOOT] 53_GETREENT_OK\n"
.Lvalist_marker:
    .ascii "[NCE-DIAG][BOOT] 54_VALIST_LOAD_OK\n"
.Lvsnprintf_r_marker:
    .ascii "[NCE-DIAG][BOOT] 55_VSNPRINTF_R_OK\n"
.Lafter_marker:
    .ascii "[NCE-DIAG][BOOT] 47_AFTER_VSNPRINTF\n"

.text
.balign 4
.global __wrap_vsnprintf
.type __wrap_vsnprintf, %function
__wrap_vsnprintf:
    sub sp, sp, #0x80
    stp x0, x1, [sp, #0x00]
    stp x2, x3, [sp, #0x10]
    str x30, [sp, #0x20]

    adrp x0, .Lbefore_marker
    add x0, x0, :lo12:.Lbefore_marker
    mov x1, #39
    bl svcOutputDebugString

    bl __getreent
    str x0, [sp, #0x40]

    adrp x0, .Lgetreent_marker
    add x0, x0, :lo12:.Lgetreent_marker
    mov x1, #32
    bl svcOutputDebugString

    ldr x3, [sp, #0x18]
    ldp q30, q31, [x3]
    stp q30, q31, [sp, #0x50]

    adrp x0, .Lvalist_marker
    add x0, x0, :lo12:.Lvalist_marker
    mov x1, #35
    bl svcOutputDebugString

    ldr x0, [sp, #0x40]
    ldr x1, [sp, #0x00]
    ldr x2, [sp, #0x08]
    ldr x3, [sp, #0x10]
    add x4, sp, #0x50
    bl _vsnprintf_r
    str x0, [sp, #0x28]

    adrp x0, .Lvsnprintf_r_marker
    add x0, x0, :lo12:.Lvsnprintf_r_marker
    mov x1, #35
    bl svcOutputDebugString

    adrp x0, .Lafter_marker
    add x0, x0, :lo12:.Lafter_marker
    mov x1, #38
    bl svcOutputDebugString

    ldr x0, [sp, #0x28]
    ldr x30, [sp, #0x20]
    add sp, sp, #0x80
    ret
.size __wrap_vsnprintf, .-__wrap_vsnprintf
