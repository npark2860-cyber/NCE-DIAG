#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <switch.h>

#include "nce_diag.h"

static uint32_t g_run_id;
static FILE *g_journal;
static const char *g_journal_path;

typedef struct LineBuilder {
    char *data;
    size_t capacity;
    size_t length;
} LineBuilder;

static void lb_init(LineBuilder *builder, char *data, size_t capacity) {
    builder->data = data;
    builder->capacity = capacity;
    builder->length = 0;
    if (capacity != 0) {
        data[0] = '\0';
    }
}

static void lb_putc(LineBuilder *builder, char value) {
    if (builder->capacity == 0 || builder->length + 1 >= builder->capacity) {
        return;
    }
    builder->data[builder->length++] = value;
    builder->data[builder->length] = '\0';
}

static void lb_puts(LineBuilder *builder, const char *text) {
    if (!text) {
        text = "(null)";
    }
    while (*text != '\0') {
        lb_putc(builder, *text++);
    }
}

static void lb_put_unsigned(
    LineBuilder *builder,
    uint64_t value,
    unsigned base,
    bool uppercase,
    unsigned minimum_width,
    bool zero_pad) {
    char digits[32];
    unsigned count = 0;
    const char *alphabet = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    do {
        digits[count++] = alphabet[value % base];
        value /= base;
    } while (value != 0 && count < sizeof(digits));

    const char padding = zero_pad ? '0' : ' ';
    while (count < minimum_width) {
        lb_putc(builder, padding);
        --minimum_width;
    }
    while (count != 0) {
        lb_putc(builder, digits[--count]);
    }
}

static void lb_put_signed(
    LineBuilder *builder,
    int64_t value,
    unsigned minimum_width,
    bool zero_pad) {
    if (value < 0) {
        lb_putc(builder, '-');
        if (minimum_width != 0) {
            --minimum_width;
        }
        const uint64_t magnitude = (uint64_t)(-(value + 1)) + 1;
        lb_put_unsigned(builder, magnitude, 10, false, minimum_width, zero_pad);
    } else {
        lb_put_unsigned(builder, (uint64_t)value, 10, false, minimum_width, zero_pad);
    }
}

static void mini_vformat(LineBuilder *builder, const char *format, va_list args) {
    while (*format != '\0') {
        if (*format != '%') {
            lb_putc(builder, *format++);
            continue;
        }

        ++format;
        if (*format == '%') {
            lb_putc(builder, '%');
            ++format;
            continue;
        }

        bool zero_pad = false;
        unsigned width = 0;
        if (*format == '0') {
            zero_pad = true;
            ++format;
        }
        while (*format >= '0' && *format <= '9') {
            width = width * 10u + (unsigned)(*format - '0');
            ++format;
        }

        enum {
            LEN_DEFAULT,
            LEN_Z,
            LEN_L,
            LEN_LL,
        } length = LEN_DEFAULT;

        if (*format == 'z') {
            length = LEN_Z;
            ++format;
        } else if (*format == 'l') {
            length = LEN_L;
            ++format;
            if (*format == 'l') {
                length = LEN_LL;
                ++format;
            }
        }

        const char spec = *format;
        if (spec == '\0') {
            lb_putc(builder, '%');
            break;
        }
        ++format;

        switch (spec) {
            case 's':
                lb_puts(builder, va_arg(args, const char *));
                break;
            case 'c':
                lb_putc(builder, (char)va_arg(args, int));
                break;
            case 'u': {
                uint64_t value;
                if (length == LEN_Z) value = (uint64_t)va_arg(args, size_t);
                else if (length == LEN_L) value = (uint64_t)va_arg(args, unsigned long);
                else if (length == LEN_LL) value = (uint64_t)va_arg(args, unsigned long long);
                else value = (uint64_t)va_arg(args, unsigned int);
                lb_put_unsigned(builder, value, 10, false, width, zero_pad);
                break;
            }
            case 'd':
            case 'i': {
                int64_t value;
                if (length == LEN_Z) value = (int64_t)va_arg(args, ptrdiff_t);
                else if (length == LEN_L) value = (int64_t)va_arg(args, long);
                else if (length == LEN_LL) value = (int64_t)va_arg(args, long long);
                else value = (int64_t)va_arg(args, int);
                lb_put_signed(builder, value, width, zero_pad);
                break;
            }
            case 'x':
            case 'X': {
                uint64_t value;
                if (length == LEN_Z) value = (uint64_t)va_arg(args, size_t);
                else if (length == LEN_L) value = (uint64_t)va_arg(args, unsigned long);
                else if (length == LEN_LL) value = (uint64_t)va_arg(args, unsigned long long);
                else value = (uint64_t)va_arg(args, unsigned int);
                lb_put_unsigned(builder, value, 16, spec == 'X', width, zero_pad);
                break;
            }
            case 'p': {
                const uintptr_t value = (uintptr_t)va_arg(args, void *);
                lb_puts(builder, "0x");
                lb_put_unsigned(builder, (uint64_t)value, 16, true,
                    (unsigned)(sizeof(uintptr_t) * 2), true);
                break;
            }
            default:
                lb_putc(builder, '%');
                lb_putc(builder, spec);
                break;
        }
    }
}

static void begin_run_line(LineBuilder *builder) {
    lb_puts(builder, "[NCE-DIAG][RUN=");
    lb_put_unsigned(builder, g_run_id, 10, false, 6, true);
    lb_puts(builder, "] ");
}

static void finish_line(LineBuilder *builder) {
    if (builder->length == 0 || builder->data[builder->length - 1] != '\n') {
        lb_putc(builder, '\n');
    }
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

void nce_diag_set_run_id(uint32_t run_id) {
    g_run_id = run_id;
}

uint32_t nce_diag_get_run_id(void) {
    return g_run_id;
}

void nce_diag_logf(const char *fmt, ...) {
    char line[352];
    LineBuilder builder;
    lb_init(&builder, line, sizeof(line));
    begin_run_line(&builder);

    va_list args;
    va_start(args, fmt);
    mini_vformat(&builder, fmt, args);
    va_end(args);

    finish_line(&builder);
    if (builder.length != 0) {
        emit_lifecycle(builder.data, builder.length);
    }
}

void nce_diag_checkpoint(const char *test_id, const char *checkpoint_id) {
    char line[352];
    LineBuilder builder;
    lb_init(&builder, line, sizeof(line));
    begin_run_line(&builder);
    lb_puts(&builder, "CKPT ");
    lb_puts(&builder, test_id);
    lb_putc(&builder, ' ');
    lb_puts(&builder, checkpoint_id);
    finish_line(&builder);
    emit_debug_only(builder.data, builder.length);
}

void nce_diag_checkpointf(
    const char *test_id,
    const char *checkpoint_id,
    const char *fmt,
    ...) {
    char line[352];
    LineBuilder builder;
    lb_init(&builder, line, sizeof(line));
    begin_run_line(&builder);
    lb_puts(&builder, "CKPT ");
    lb_puts(&builder, test_id);
    lb_putc(&builder, ' ');
    lb_puts(&builder, checkpoint_id);

    if (fmt && *fmt != '\0') {
        lb_putc(&builder, ' ');
        va_list args;
        va_start(args, fmt);
        mini_vformat(&builder, fmt, args);
        va_end(args);
    }

    finish_line(&builder);
    emit_debug_only(builder.data, builder.length);
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
