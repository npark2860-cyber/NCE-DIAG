.section .rodata
.balign 4
.Lbefore_vsnprintf_marker:
    .ascii "[NCE-DIAG][BOOT] 46_BEFORE_VSNPRINTF\n"
.Lafter_vsnprintf_marker:
    .ascii "[NCE-DIAG][BOOT] 47_AFTER_VSNPRINTF\n"

.text
.balign 4
.global __wrap_vsnprintf
.type __wrap_vsnprintf, %function
__wrap_vsnprintf:
    sub sp, sp, #0x40
    stp x0, x1, [sp, #0x00]
    stp x2, x3, [sp, #0x10]
    str x30, [sp, #0x20]

    adrp x0, .Lbefore_vsnprintf_marker
    add x0, x0, :lo12:.Lbefore_vsnprintf_marker
    mov x1, #39
    bl svcOutputDebugString

    ldp x0, x1, [sp, #0x00]
    ldp x2, x3, [sp, #0x10]
    bl __real_vsnprintf
    str x0, [sp, #0x28]

    adrp x0, .Lafter_vsnprintf_marker
    add x0, x0, :lo12:.Lafter_vsnprintf_marker
    mov x1, #38
    bl svcOutputDebugString

    ldr x0, [sp, #0x28]
    ldr x30, [sp, #0x20]
    add sp, sp, #0x40
    ret
.size __wrap_vsnprintf, .-__wrap_vsnprintf
