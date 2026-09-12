.section .rodata
.balign 4
.Lbefore_vsnprintf_marker:
    .ascii "[NCE-DIAG][BOOT] 46_BEFORE_VSNPRINTF\n"
.Ltls_mrs_ok_marker:
    .ascii "[NCE-DIAG][BOOT] 48_TLS_MRS_OK\n"
.Ltls_slot_ok_marker:
    .ascii "[NCE-DIAG][BOOT] 49_TLS_SLOT_LOAD_OK\n"
.Ltls_magic_ok_marker:
    .ascii "[NCE-DIAG][BOOT] 50_TLS_MAGIC_OK\n"
.Ltls_magic_bad_marker:
    .ascii "[NCE-DIAG][BOOT] 50_TLS_MAGIC_BAD\n"
.Ltls_reent_ptr_ok_marker:
    .ascii "[NCE-DIAG][BOOT] 51_TLS_REENT_PTR_OK\n"
.Ltls_reent_deref_ok_marker:
    .ascii "[NCE-DIAG][BOOT] 52_TLS_REENT_DEREF_OK\n"
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

    mrs x9, tpidrro_el0
    str x9, [sp, #0x30]

    adrp x0, .Ltls_mrs_ok_marker
    add x0, x0, :lo12:.Ltls_mrs_ok_marker
    mov x1, #31
    bl svcOutputDebugString

    ldr x9, [sp, #0x30]
    ldr w10, [x9, #0x1e0]
    str x10, [sp, #0x38]

    adrp x0, .Ltls_slot_ok_marker
    add x0, x0, :lo12:.Ltls_slot_ok_marker
    mov x1, #37
    bl svcOutputDebugString

    ldr w10, [sp, #0x38]
    mov w11, #0x5624
    movk w11, #0x2154, lsl #16
    cmp w10, w11
    b.ne .Ltls_magic_bad

    adrp x0, .Ltls_magic_ok_marker
    add x0, x0, :lo12:.Ltls_magic_ok_marker
    mov x1, #33
    bl svcOutputDebugString

    ldr x9, [sp, #0x30]
    ldr x10, [x9, #0x1f0]
    str x10, [sp, #0x38]

    adrp x0, .Ltls_reent_ptr_ok_marker
    add x0, x0, :lo12:.Ltls_reent_ptr_ok_marker
    mov x1, #37
    bl svcOutputDebugString

    ldr x10, [sp, #0x38]
    ldr x11, [x10]

    adrp x0, .Ltls_reent_deref_ok_marker
    add x0, x0, :lo12:.Ltls_reent_deref_ok_marker
    mov x1, #39
    bl svcOutputDebugString
    b .Lcall_real

.Ltls_magic_bad:
    adrp x0, .Ltls_magic_bad_marker
    add x0, x0, :lo12:.Ltls_magic_bad_marker
    mov x1, #34
    bl svcOutputDebugString

.Lcall_real:
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
