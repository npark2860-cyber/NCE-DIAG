#include <switch.h>

/*
 * Runtime-only boundary probe.
 *
 * This is an exact structural copy of libnx 4.12.0 __libnx_init(), with
 * diagnostic markers added only after __libc_init_array() has returned.
 * The goal is to distinguish "crash in/after remaining constructors" from
 * "crash after libnx init returns on the crt0 -> main path" without changing
 * testcase code or service policy.
 */

void envSetup(void *ctx, Handle main_thread, void *saved_lr);
void newlibSetup(void);
void virtmemSetup(void);
void __libnx_initheap(void);
void __libnx_init_thread(void);
void argvSetup(void);
void __appInit(void);
void __libc_init_array(void);

#define LIBNX_INIT_MARKER(text)                                                  \
    do {                                                                         \
        static const char marker[] = "[NCE-DIAG][BOOT] " text "\n";            \
        (void)svcOutputDebugString(marker, (u64)(sizeof(marker) - 1));            \
    } while (0)

void __libnx_init(void *ctx, Handle main_thread, void *saved_lr) {
    envSetup(ctx, main_thread, saved_lr);
    newlibSetup();
    virtmemSetup();
    __libnx_initheap();
    __libnx_init_thread();
    argvSetup();

    __appInit();
    __libc_init_array();

    LIBNX_INIT_MARKER("50_LIBC_INIT_ARRAY_DONE");
    __asm__ volatile("" ::: "memory");
    LIBNX_INIT_MARKER("51_LIBNX_INIT_RETURN");
}
