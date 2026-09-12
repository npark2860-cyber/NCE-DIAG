#include <stdbool.h>
#include <stddef.h>

#include <switch.h>

#include "nce_diag.h"

static bool g_sm_ready;
static bool g_fs_ready;
static bool g_sd_mounted;

#define BOOT_MARKER(text)                                                       \
    do {                                                                        \
        static const char marker[] = "[NCE-DIAG][BOOT] " text "\n";           \
        (void)svcOutputDebugString(marker, (u64)(sizeof(marker) - 1));           \
    } while (0)

void __appInit(void) {
    BOOT_MARKER("00_APP_INIT_ENTER");
    BOOT_MARKER("10_PRE_SM_INIT");

    const Result rc = smInitialize();
    if (R_SUCCEEDED(rc)) {
        g_sm_ready = true;
        BOOT_MARKER("20_POST_SM_INIT");
    } else {
        g_sm_ready = false;
        BOOT_MARKER("21_SM_INIT_FAILED");
    }

    BOOT_MARKER("30_APP_INIT_DONE");
}

void __appExit(void) {
    BOOT_MARKER("90_APP_EXIT_ENTER");

    if (g_sd_mounted) {
        fsdevUnmountAll();
        g_sd_mounted = false;
    }

    if (g_fs_ready) {
        fsExit();
        g_fs_ready = false;
    }

    if (g_sm_ready) {
        smExit();
        g_sm_ready = false;
    }

    BOOT_MARKER("99_APP_EXIT_DONE");
}

bool nce_diag_sm_ready(void) {
    return g_sm_ready;
}

bool nce_diag_filesystem_enable(void) {
    if (g_sd_mounted) {
        return true;
    }

    if (!g_fs_ready) {
        nce_diag_logf("OPTIONAL_FS 10_PRE_FS_INIT");
        const Result fs_rc = fsInitialize();
        nce_diag_logf("OPTIONAL_FS 20_POST_FS_INIT rc=0x%08X", (unsigned)fs_rc);
        if (R_FAILED(fs_rc)) {
            return false;
        }
        g_fs_ready = true;
    }

    nce_diag_logf("OPTIONAL_FS 30_PRE_SD_MOUNT");
    const Result mount_rc = fsdevMountSdmc();
    nce_diag_logf("OPTIONAL_FS 40_POST_SD_MOUNT rc=0x%08X", (unsigned)mount_rc);
    if (R_FAILED(mount_rc)) {
        fsExit();
        g_fs_ready = false;
        return false;
    }

    g_sd_mounted = true;
    return true;
}

void nce_diag_filesystem_disable(void) {
    if (g_sd_mounted) {
        nce_diag_logf("OPTIONAL_FS 90_PRE_UNMOUNT");
        fsdevUnmountAll();
        g_sd_mounted = false;
        nce_diag_logf("OPTIONAL_FS 91_POST_UNMOUNT");
    }

    if (g_fs_ready) {
        nce_diag_logf("OPTIONAL_FS 92_PRE_FS_EXIT");
        fsExit();
        g_fs_ready = false;
        nce_diag_logf("OPTIONAL_FS 93_POST_FS_EXIT");
    }
}
