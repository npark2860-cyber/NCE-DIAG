#include <switch.h>

/*
 * Runtime-only SVC 0x27 count/call-site probe.
 *
 * Eden Windows NCE reaches all four __appInit debug markers.  With two
 * constructor markers, the host dies while re-entering the guest post-handler
 * for the second constructor marker.  Keep exactly one constructor marker so
 * main()'s existing START log becomes the sixth svcOutputDebugString call.
 *
 * This distinguishes a sixth-SVC/reentry accumulation problem from a problem
 * specific to the second constructor call-site.  The volatile post-call store
 * prevents this remaining debug call from being tail-called.
 */
static volatile u32 g_pre_main_probe_state;

__attribute__((constructor(101))) static void nce_diag_pre_main_constructor_probe(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 40_CTOR_ENTER\n";

    g_pre_main_probe_state = 0x40;
    (void)svcOutputDebugString(enter, (u64)(sizeof(enter) - 1));
    g_pre_main_probe_state = 0x41;
}
