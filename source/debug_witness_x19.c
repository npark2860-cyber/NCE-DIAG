#include <stdint.h>

#include <switch.h>

uint32_t nce_diag_emit_debug_witness_x19(
    const char *text,
    uint64_t length,
    uint64_t witness);

/*
 * x20 is intentionally left untouched. Eden's current Windows NCE diagnostic
 * probes state relative to x20 during SVC re-entry, so changing x20 creates
 * observer-induced unmapped reads. Only x19 carries the checkpoint witness.
 */
__asm__(
    ".text\n"
    ".balign 4\n"
    ".global nce_diag_emit_debug_witness_x19\n"
    ".type nce_diag_emit_debug_witness_x19, %function\n"
    "nce_diag_emit_debug_witness_x19:\n"
    "    str x19, [sp, #-0x10]!\n"
    "    str x30, [sp, #0x8]\n"
    "    cbz x2, 1f\n"
    "    mov x19, x2\n"
    "1:\n"
    "    bl svcOutputDebugString\n"
    "    ldr x30, [sp, #0x8]\n"
    "    ldr x19, [sp], #0x10\n"
    "    ret\n"
    ".size nce_diag_emit_debug_witness_x19, .-nce_diag_emit_debug_witness_x19\n");
