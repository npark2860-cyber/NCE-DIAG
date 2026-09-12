#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Crash-observability bootstrap override.
 *
 * main() used armGetSystemTick() to derive a run id before its first marker.
 * libnx implements that helper as MRS CNTPCT_EL0, which is itself an NCE
 * architectural path and therefore must not run before the harness can report
 * that main was entered.  Keep the existing call site intact but replace it
 * inside NCE-DIAG sources with address-derived entropy that uses ordinary guest
 * integer/address instructions only.
 */
static inline uint64_t nce_diag_bootstrap_run_entropy(void) {
    uintptr_t stack_entropy = (uintptr_t)&stack_entropy;
    uintptr_t code_entropy = (uintptr_t)&nce_diag_bootstrap_run_entropy;
    uint64_t value = (uint64_t)(stack_entropy ^ code_entropy);
    value ^= value >> 17;
    value ^= value << 11;
    return value;
}

#define armGetSystemTick() nce_diag_bootstrap_run_entropy()

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

bool nce_diag_sm_ready(void);
bool nce_diag_filesystem_enable(void);
void nce_diag_filesystem_disable(void);

uint32_t nce_diag_cpu_nzcv_csel_probe(uint64_t *nzcv_before, uint64_t *nzcv_after);
uint32_t nce_diag_cpu_reg_preserve_probe(void);

TestResult run_cpu_nzcv_cinc_001(void);
TestResult run_cpu_nzcv_csel_001(void);
TestResult run_cpu_reg_preserve_001(void);
TestResult run_ipc_sm_get_service_001(void);
TestResult run_ipc_svc21_repeated_001(void);

/*
 * All project-owned debug output is routed through a witness wrapper.  For
 * lines containing " CKPT <test> <checkpoint>", the wrapper temporarily loads
 * x19/x20 with NCET/NCEC hash tags before issuing SVC 0x27, then restores the
 * original callee-saved registers after the SVC returns.  Eden's existing NCE
 * RunThread diagnostic already records x19/x20 on SVC entry, so a host crash
 * inside SVC 0x27 still leaves an independently decodable checkpoint witness.
 */
uint32_t nce_diag_output_debug_string(const char *text, uint64_t length);
#define svcOutputDebugString(text, length) \
    nce_diag_output_debug_string((text), (uint64_t)(length))
