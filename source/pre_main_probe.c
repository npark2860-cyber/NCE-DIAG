#include <switch.h>

/*
 * Runtime-only boundary probe.
 *
 * libnx calls __libc_init_array() after project-owned __appInit() and before
 * main().  The 0.2.1 Eden logs reach the final __appInit marker but terminate
 * before main's first normal diagnostic marker.  Keep this constructor free of
 * stdio, heap allocation, filesystem, SM IPC, and NCE-DIAG formatting so the
 * only observable operation is SVC 0x27 itself.
 *
 * Important: keep both debug calls as ordinary BL calls.  A previous build let
 * the compiler tail-call the second svcOutputDebugString with B, and Eden died
 * at the corresponding post-SVC RET seam.  The volatile post-call store below
 * intentionally prevents that tail-call so this probe tests the SVC return
 * path without changing testcase code.
 */
static volatile u32 g_pre_main_probe_state;

__attribute__((constructor(101))) static void nce_diag_pre_main_constructor_probe(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 40_CTOR_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 41_CTOR_DONE\n";

    g_pre_main_probe_state = 0x40;
    (void)svcOutputDebugString(enter, (u64)(sizeof(enter) - 1));
    g_pre_main_probe_state = 0x41;
    (void)svcOutputDebugString(done, (u64)(sizeof(done) - 1));
    g_pre_main_probe_state = 0x42;
}
