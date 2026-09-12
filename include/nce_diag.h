#pragma once

#include <stdint.h>

typedef enum TestStatus {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2,
} TestStatus;

typedef struct TestResult {
    const char *id;
    TestStatus status;
    uint64_t expected;
    uint64_t actual;
    uint64_t nzcv_before;
    uint64_t nzcv_after;
    const char *detail;
} TestResult;

uint32_t nce_diag_cpu_nzcv_cinc_probe(
    uint32_t input,
    uint32_t *scratch,
    uint64_t *nzcv_before,
    uint64_t *nzcv_after);

TestResult run_cpu_nzcv_cinc_001(void);
