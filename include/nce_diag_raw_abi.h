#pragma once

#include <stddef.h>
#include <stdint.h>

#define NCE_DIAG_RAW_ABI_VERSION 1u

typedef struct NceDiagCpuNzcvCincRawResult {
    uint32_t abi_version;      /* 0x00 */
    uint32_t input_w16;        /* 0x04 */
    uint32_t actual_result;    /* 0x08 */
    uint32_t scratch_w17;      /* 0x0c */
    uint32_t scratch_w18;      /* 0x10 */
    uint32_t reserved;         /* 0x14 */
    uint64_t nzcv_before;      /* 0x18 */
    uint64_t nzcv_after;       /* 0x20 */
} NceDiagCpuNzcvCincRawResult;

_Static_assert(offsetof(NceDiagCpuNzcvCincRawResult, scratch_w17) == 0x0c, "raw ABI must preserve STP +0x0c shape");
_Static_assert(offsetof(NceDiagCpuNzcvCincRawResult, nzcv_before) == 0x18, "raw ABI nzcv_before offset changed");
_Static_assert(offsetof(NceDiagCpuNzcvCincRawResult, nzcv_after) == 0x20, "raw ABI nzcv_after offset changed");
_Static_assert(sizeof(NceDiagCpuNzcvCincRawResult) == 0x28, "raw ABI result size changed");
