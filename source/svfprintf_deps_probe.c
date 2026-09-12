#include <stddef.h>
#include <stdint.h>

#include <switch.h>

volatile uint32_t g_svfprintf_probe_active;

static inline void probe_mark(const char *text, size_t len) {
    if (g_svfprintf_probe_active) {
        (void)svcOutputDebugString(text, (u64)len);
    }
}

extern void *__real__localeconv_r(void *reent);
extern size_t __real_strlen(const char *s);
extern void *__real_memset(void *dst, int c, size_t n);
extern void *__real___getreent(void);
extern int __real___locale_mb_cur_max(void);
extern void *__real__malloc_r(void *reent, size_t n);

void *__wrap__localeconv_r(void *reent) {
    static const char enter[] = "[NCE-DIAG][BOOT] 58_LOCALECONV_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 59_LOCALECONV_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real__localeconv_r(reent);
    }
    probe_mark(enter, sizeof(enter) - 1);
    void *result = __real__localeconv_r(reent);
    probe_mark(done, sizeof(done) - 1);
    return result;
}

size_t __wrap_strlen(const char *s) {
    static const char enter[] = "[NCE-DIAG][BOOT] 60_STRLEN_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 61_STRLEN_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real_strlen(s);
    }
    probe_mark(enter, sizeof(enter) - 1);
    size_t result = __real_strlen(s);
    probe_mark(done, sizeof(done) - 1);
    return result;
}

void *__wrap_memset(void *dst, int c, size_t n) {
    static const char enter[] = "[NCE-DIAG][BOOT] 62_MEMSET_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 63_MEMSET_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real_memset(dst, c, n);
    }
    probe_mark(enter, sizeof(enter) - 1);
    void *result = __real_memset(dst, c, n);
    probe_mark(done, sizeof(done) - 1);
    return result;
}

void *__wrap___getreent(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 64_GETREENT_INNER_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 65_GETREENT_INNER_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real___getreent();
    }
    probe_mark(enter, sizeof(enter) - 1);
    void *result = __real___getreent();
    probe_mark(done, sizeof(done) - 1);
    return result;
}

int __wrap___locale_mb_cur_max(void) {
    static const char enter[] = "[NCE-DIAG][BOOT] 66_MB_CUR_MAX_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 67_MB_CUR_MAX_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real___locale_mb_cur_max();
    }
    probe_mark(enter, sizeof(enter) - 1);
    int result = __real___locale_mb_cur_max();
    probe_mark(done, sizeof(done) - 1);
    return result;
}

void *__wrap__malloc_r(void *reent, size_t n) {
    static const char enter[] = "[NCE-DIAG][BOOT] 68_MALLOC_R_ENTER\n";
    static const char done[] = "[NCE-DIAG][BOOT] 69_MALLOC_R_RETURN\n";
    if (!g_svfprintf_probe_active) {
        return __real__malloc_r(reent, n);
    }
    probe_mark(enter, sizeof(enter) - 1);
    void *result = __real__malloc_r(reent, n);
    probe_mark(done, sizeof(done) - 1);
    return result;
}
