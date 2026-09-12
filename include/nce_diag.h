#pragma once

#include <stdbool.h>
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

typedef TestResult (*NceDiagTestFn)(void);

typedef struct NceDiagTestCase {
    const char *id;
    NceDiagTestFn run;
} NceDiagTestCase;

void nce_diag_set_run_id(uint32_t run_id);
uint32_t nce_diag_get_run_id(void);
void nce_diag_logf(const char *fmt, ...);
void nce_diag_checkpoint(const char *test_id, const char *checkpoint_id);
void nce_diag_checkpointf(const char *test_id, const char *checkpoint_id, const char *fmt, ...);
bool nce_diag_persistence_enable(void);
void nce_diag_persistence_disable(void);

uint32_t nce_diag_cpu_nzcv_csel_probe(uint64_t *nzcv_before, uint64_t *nzcv_after);
uint32_t nce_diag_cpu_reg_preserve_probe(void);

TestResult run_cpu_nzcv_cinc_001(void);
TestResult run_cpu_nzcv_csel_001(void);
TestResult run_cpu_reg_preserve_001(void);
TestResult run_ipc_sm_get_service_001(void);
TestResult run_ipc_svc21_repeated_001(void);
