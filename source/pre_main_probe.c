#include <switch.h>

/*
 * Runtime-only post-SVC epilogue probe.
 *
 * The two-marker non-tail constructor reaches the second svcOutputDebugString
 * post-handler with the expected guest LR (returning to the instruction after
 * the call), but the host dies before the constructor can finish.  Keep the
 * first post-call volatile store exactly as the known-good control, then make
 * the second call non-tail with a real NOP instead of touching the state word
 * after it.  This isolates the first instruction after the failing SVC return:
 * if execution progresses, the x20-based post-call store was the trigger; if
 * it still dies, the remaining constructor epilogue/return boundary is next.
 */
static volatile u32 g_pre_main_probe_state;

__attribute__((constructor(101))) static void nce_diag_pre_main_constructor_probe(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 40_CTOR_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 41_CTOR_DONE\n";

    g_pre_main_probe_state = 0x40;
    (void)svcOutputDebugString(enter, (u64)(sizeof(enter) - 1));
    g_pre_main_probe_state = 0x41;
    (void)svcOutputDebugString(done, (u64)(sizeof(done) - 1));
    __asm__ volatile("nop");
}
