#include <switch.h>

/*
 * Runtime-only boundary probe.
 *
 * libnx calls __libc_init_array() after project-owned __appInit() and before
 * main().  The 0.2.1 Eden logs reach the final __appInit marker but terminate
 * before main's first normal diagnostic marker.  Keep this constructor free of
 * stdio, heap allocation, filesystem, SM IPC, and NCE-DIAG formatting so the
 * only observable operation is SVC 0x27 itself.
 */
__attribute__((constructor(101))) static void nce_diag_pre_main_constructor_probe(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 40_CTOR_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 41_CTOR_DONE\n";

    (void)svcOutputDebugString(enter, (u64)(sizeof(enter) - 1));
    __asm__ volatile("" ::: "memory");
    (void)svcOutputDebugString(done, (u64)(sizeof(done) - 1));
}
