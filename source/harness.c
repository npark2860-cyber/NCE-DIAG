#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <switch.h>

#include "nce_diag.h"

static uint32_t g_run_id;
static FILE *g_journal;
static const char *g_journal_path;

void nce_diag_set_run_id(uint32_t run_id) {
    g_run_id = run_id;
}

uint32_t nce_diag_get_run_id(void) {
    return g_run_id;
}

static void emit_line(const char *line, size_t len) {
    (void)svcOutputDebugString(line, (u64)len);

    if (g_journal) {
        (void)fwrite(line, 1, len, g_journal);
        (void)fflush(g_journal);
    }
}

void nce_diag_logf(const char *fmt, ...) {
    char body[256];
    char line[352];

    va_list args;
    va_start(args, fmt);
    const int body_rc = vsnprintf(body, sizeof(body), fmt, args);
    va_end(args);

    if (body_rc < 0) {
        return;
    }

    int line_rc = snprintf(
        line,
        sizeof(line),
        "[NCE-DIAG][RUN=%06u] %s",
        g_run_id,
        body);

    if (line_rc < 0) {
        return;
    }

    size_t len = (size_t)line_rc;
    if (len >= sizeof(line)) {
        len = sizeof(line) - 1;
    }

    if (len == 0 || line[len - 1] != '\n') {
        if (len + 1 < sizeof(line)) {
            line[len++] = '\n';
            line[len] = '\0';
        }
    }

    emit_line(line, len);
}

void nce_diag_checkpoint(const char *test_id, const char *checkpoint_id) {
    nce_diag_logf("CKPT %s %s", test_id, checkpoint_id);
}

void nce_diag_checkpointf(
    const char *test_id,
    const char *checkpoint_id,
    const char *fmt,
    ...) {
    char detail[128];

    va_list args;
    va_start(args, fmt);
    const int rc = vsnprintf(detail, sizeof(detail), fmt, args);
    va_end(args);

    if (rc < 0) {
        nce_diag_checkpoint(test_id, checkpoint_id);
        return;
    }

    nce_diag_logf("CKPT %s %s %s", test_id, checkpoint_id, detail);
}

bool nce_diag_persistence_enable(void) {
    static const char *const paths[] = {
        "sdmc:/nce_diag_journal.log",
        "nce_diag_journal.log",
    };

    if (g_journal) {
        return true;
    }

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        FILE *fp = fopen(paths[i], "a");
        if (fp) {
            g_journal = fp;
            g_journal_path = paths[i];
            nce_diag_logf("PERSISTENCE ENABLED path=%s", g_journal_path);
            return true;
        }
    }

    nce_diag_logf("PERSISTENCE ENABLE_FAILED");
    return false;
}

void nce_diag_persistence_disable(void) {
    if (!g_journal) {
        return;
    }

    nce_diag_logf("PERSISTENCE FLUSH_CLOSE path=%s", g_journal_path);
    (void)fflush(g_journal);
    (void)fclose(g_journal);
    g_journal = NULL;
    g_journal_path = NULL;
}
