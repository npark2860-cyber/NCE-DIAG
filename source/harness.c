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

static size_t format_line(char *line, size_t line_size, const char *fmt, va_list args) {
    char body[256];
    const int body_rc = vsnprintf(body, sizeof(body), fmt, args);
    if (body_rc < 0) {
        return 0;
    }

    const int line_rc = snprintf(
        line,
        line_size,
        "[NCE-DIAG][RUN=%06u] %s",
        g_run_id,
        body);

    if (line_rc < 0) {
        return 0;
    }

    size_t len = (size_t)line_rc;
    if (len >= line_size) {
        len = line_size - 1;
    }

    if (len == 0 || line[len - 1] != '\n') {
        if (len + 1 < line_size) {
            line[len++] = '\n';
            line[len] = '\0';
        }
    }

    return len;
}

static void emit_debug_only(const char *line, size_t len) {
    (void)svcOutputDebugString(line, (u64)len);
}

static void emit_lifecycle(const char *line, size_t len) {
    emit_debug_only(line, len);

    if (g_journal) {
        (void)fwrite(line, 1, len, g_journal);
        (void)fflush(g_journal);
    }
}

void nce_diag_logf(const char *fmt, ...) {
    char line[352];

    va_list args;
    va_start(args, fmt);
    const size_t len = format_line(line, sizeof(line), fmt, args);
    va_end(args);

    if (len != 0) {
        emit_lifecycle(line, len);
    }
}

void nce_diag_checkpoint(const char *test_id, const char *checkpoint_id) {
    char line[352];
    const int rc = snprintf(
        line,
        sizeof(line),
        "[NCE-DIAG][RUN=%06u] CKPT %s %s\n",
        g_run_id,
        test_id,
        checkpoint_id);

    if (rc <= 0) {
        return;
    }

    size_t len = (size_t)rc;
    if (len >= sizeof(line)) {
        len = sizeof(line) - 1;
    }

    // Checkpoints are deliberately debug-only. Persistent file I/O here could
    // overwrite the TLS IPC request buffer between REQUEST_BUILT and PRE_SVC.
    emit_debug_only(line, len);
}

void nce_diag_checkpointf(
    const char *test_id,
    const char *checkpoint_id,
    const char *fmt,
    ...) {
    char detail[128];
    char line[352];

    va_list args;
    va_start(args, fmt);
    const int detail_rc = vsnprintf(detail, sizeof(detail), fmt, args);
    va_end(args);

    if (detail_rc < 0) {
        nce_diag_checkpoint(test_id, checkpoint_id);
        return;
    }

    const int rc = snprintf(
        line,
        sizeof(line),
        "[NCE-DIAG][RUN=%06u] CKPT %s %s %s\n",
        g_run_id,
        test_id,
        checkpoint_id,
        detail);

    if (rc <= 0) {
        return;
    }

    size_t len = (size_t)rc;
    if (len >= sizeof(line)) {
        len = sizeof(line) - 1;
    }

    emit_debug_only(line, len);
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
