#include <stddef.h>
#include <stdint.h>

#include <switch.h>

#define NCE_DIAG_TEST_TAG_PREFIX UINT64_C(0x4E43455400000000)
#define NCE_DIAG_CKPT_TAG_PREFIX UINT64_C(0x4E43454300000000)
#define NCE_DIAG_TAG_LOW_MASK    UINT64_C(0x00000000FFFFFFFF)

extern uint32_t nce_diag_emit_debug_witness(
    const char *text,
    uint64_t length,
    uint64_t test_tag,
    uint64_t checkpoint_tag);

__asm__(
    ".text\n"
    ".balign 4\n"
    ".global nce_diag_emit_debug_witness\n"
    ".type nce_diag_emit_debug_witness, %function\n"
    "nce_diag_emit_debug_witness:\n"
    "    stp x19, x20, [sp, #-0x20]!\n"
    "    str x30, [sp, #0x10]\n"
    "    cbz x2, 1f\n"
    "    mov x19, x2\n"
    "    mov x20, x3\n"
    "1:\n"
    "    bl svcOutputDebugString\n"
    "    ldr x30, [sp, #0x10]\n"
    "    ldp x19, x20, [sp], #0x20\n"
    "    ret\n"
    ".size nce_diag_emit_debug_witness, .-nce_diag_emit_debug_witness\n");

static uint32_t fnv1a32_range(const char *begin, const char *end) {
    uint32_t hash = UINT32_C(2166136261);
    for (const char *p = begin; p < end; ++p) {
        hash ^= (uint8_t)*p;
        hash *= UINT32_C(16777619);
    }
    return hash;
}

static int byte_is_token_end(char value) {
    return value == '\0' || value == ' ' || value == '\t' ||
           value == '\r' || value == '\n';
}

static const char *find_checkpoint_marker(const char *text, uint64_t length) {
    static const char marker[] = " CKPT ";
    const size_t marker_length = sizeof(marker) - 1;

    if (!text || length < marker_length) {
        return NULL;
    }

    for (uint64_t i = 0; i + marker_length <= length; ++i) {
        size_t j = 0;
        while (j < marker_length && text[i + j] == marker[j]) {
            ++j;
        }
        if (j == marker_length) {
            return text + i + marker_length;
        }
    }
    return NULL;
}

static void derive_checkpoint_tags(
    const char *text,
    uint64_t length,
    uint64_t *test_tag,
    uint64_t *checkpoint_tag) {
    *test_tag = 0;
    *checkpoint_tag = 0;

    const char *const after_marker = find_checkpoint_marker(text, length);
    if (!after_marker) {
        return;
    }

    const char *const limit = text + length;
    const char *test_begin = after_marker;
    const char *test_end = test_begin;
    while (test_end < limit && !byte_is_token_end(*test_end)) {
        ++test_end;
    }
    if (test_end == test_begin || test_end >= limit) {
        return;
    }

    const char *checkpoint_begin = test_end;
    while (checkpoint_begin < limit &&
           (*checkpoint_begin == ' ' || *checkpoint_begin == '\t')) {
        ++checkpoint_begin;
    }
    const char *checkpoint_end = checkpoint_begin;
    while (checkpoint_end < limit && !byte_is_token_end(*checkpoint_end)) {
        ++checkpoint_end;
    }
    if (checkpoint_end == checkpoint_begin) {
        return;
    }

    const uint32_t test_hash = fnv1a32_range(test_begin, test_end);
    const uint32_t checkpoint_hash = fnv1a32_range(checkpoint_begin, checkpoint_end);

    *test_tag = NCE_DIAG_TEST_TAG_PREFIX |
                ((uint64_t)test_hash & NCE_DIAG_TAG_LOW_MASK);
    *checkpoint_tag = NCE_DIAG_CKPT_TAG_PREFIX |
                      ((uint64_t)checkpoint_hash & NCE_DIAG_TAG_LOW_MASK);
}

uint32_t nce_diag_output_debug_string(const char *text, uint64_t length) {
    uint64_t test_tag = 0;
    uint64_t checkpoint_tag = 0;
    derive_checkpoint_tags(text, length, &test_tag, &checkpoint_tag);
    return nce_diag_emit_debug_witness(text, length, test_tag, checkpoint_tag);
}
