#pragma once

#include <stddef.h>
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
    uint32_t result_code;
    const char *detail;
} TestResult;

uint32_t nce_diag_cpu_nzcv_csel_probe(uint64_t *nzcv_before, uint64_t *nzcv_after);
uint32_t nce_diag_cpu_reg_preserve_probe(void);

TestResult run_cpu_nzcv_cinc_001(void);
TestResult run_cpu_nzcv_csel_001(void);
TestResult run_cpu_reg_preserve_001(void);
TestResult run_ipc_sm_get_service_001(void);
TestResult run_ipc_svc21_repeated_001(void);
