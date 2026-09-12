#include <switch.h>

static volatile u32 g_pre_main_probe_state;

__attribute__((constructor)) static void nce_diag_pre_main_constructor_probe(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 40_CTOR_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 41_CTOR_DONE\n";
    static const char witness[] = "[NCE-DIAG][BOOT] 42_POST_SECOND_RETURN\n";

    g_pre_main_probe_state = 0x40;
    (void)svcOutputDebugString(enter, (u64)(sizeof(enter) - 1));
    g_pre_main_probe_state = 0x41;
    (void)svcOutputDebugString(done, (u64)(sizeof(done) - 1));
    (void)svcOutputDebugString(witness, (u64)(sizeof(witness) - 1));
    __asm__ volatile("nop");
}
