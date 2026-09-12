#include <stddef.h>
#include <stdint.h>

#define NCE_DIAG_WITNESS_TAG_PREFIX UINT64_C(0x4E43455700000000)
#define NCE_DIAG_TAG_LOW_MASK        UINT64_C(0x00000000FFFFFFFF)

extern uint32_t nce_diag_emit_debug_witness(
    const char *text,
    uint64_t length,
    uint64_t test_tag,
    uint64_t checkpoint_tag);

static uint32_t fnv1a32_update(uint32_t hash, const char *begin, const char *end) {
    for (const char *p = begin; p < end; ++p) {
        hash ^= (uint8_t)*p;
        hash *= UINT32_C(16777619);
    }
    return hash;
}

static int token_end(char value) {
    return value == '\0' || value == ' ' || value == '\t' ||
           value == '\r' || value == '\n';
}

static const char *find_ckpt(const char *text, uint64_t length) {
    static const char marker[] = " CKPT ";
    const size_t marker_len = sizeof(marker) - 1;
    if (!text || length < marker_len) return NULL;

    for (uint64_t i = 0; i + marker_len <= length; ++i) {
        size_t j = 0;
        while (j < marker_len && text[i + j] == marker[j]) ++j;
        if (j == marker_len) return text + i + marker_len;
    }
    return NULL;
}

static uint64_t combined_tag(const char *text, uint64_t length) {
    const char *after = find_ckpt(text, length);
    if (!after) return 0;

    const char *limit = text + length;
    const char *test_begin = after;
    const char *test_end = test_begin;
    while (test_end < limit && !token_end(*test_end)) ++test_end;
    if (test_end == test_begin || test_end >= limit) return 0;

    const char *ckpt_begin = test_end;
    while (ckpt_begin < limit && (*ckpt_begin == ' ' || *ckpt_begin == '\t')) ++ckpt_begin;
    const char *ckpt_end = ckpt_begin;
    while (ckpt_end < limit && !token_end(*ckpt_end)) ++ckpt_end;
    if (ckpt_end == ckpt_begin) return 0;

    uint32_t hash = UINT32_C(2166136261);
    hash = fnv1a32_update(hash, test_begin, test_end);
    hash ^= (uint8_t)' ';
    hash *= UINT32_C(16777619);
    hash = fnv1a32_update(hash, ckpt_begin, ckpt_end);

    return NCE_DIAG_WITNESS_TAG_PREFIX |
           ((uint64_t)hash & NCE_DIAG_TAG_LOW_MASK);
}

uint32_t nce_diag_output_debug_string_safe(const char *text, uint64_t length) {
    const uint64_t witness = combined_tag(text, length);
    const uint64_t safe_x20 = witness ? (uint64_t)(uintptr_t)text : 0;
    return nce_diag_emit_debug_witness(text, length, witness, safe_x20);
}
