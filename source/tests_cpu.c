#include <stdbool.h>
#include <stdint.h>

#include "nce_diag.h"
#include "nce_diag_raw_abi.h"

extern NceDiagCpuNzcvCincRawResult *nce_diag_raw_cpu_nzcv_cinc_001(
    NceDiagCpuNzcvCincRawResult *result);

TestResult run_cpu_nzcv_cinc_001(void) {
    const uint64_t expected_nzcv = UINT64_C(0x60000000);
    NceDiagCpuNzcvCincRawResult raw = {0};

    nce_diag_raw_cpu_nzcv_cinc_001(&raw);

    const bool pass =
        raw.abi_version == NCE_DIAG_RAW_ABI_VERSION &&
        raw.input_w16 == 0 &&
        raw.actual_result == 2 &&
        raw.scratch_w17 == 0x1111 &&
        raw.scratch_w18 == 0x2222 &&
        raw.nzcv_before == expected_nzcv &&
        raw.nzcv_after == expected_nzcv;

    return (TestResult){
        .id = "CPU.NZCV.CINC.001",
        .status = pass ? TEST_PASS : TEST_FAIL,
        .expected = 2,
        .actual = raw.actual_result,
        .nzcv_before = raw.nzcv_before,
        .nzcv_after = raw.nzcv_after,
        .result_code = 0,
        .detail = "embedded raw cmp/stp-x18/cinc path",
    };
}

TestResult run_cpu_nzcv_csel_001(void) {
    const uint64_t expected_nzcv = UINT64_C(0x80000000);
    uint64_t nzcv_before = 0;
    uint64_t nzcv_after = 0;
    const uint32_t actual = nce_diag_cpu_nzcv_csel_probe(&nzcv_before, &nzcv_after);

    const bool pass =
        actual == 5 &&
        nzcv_before == expected_nzcv &&
        nzcv_after == expected_nzcv;

    return (TestResult){
        .id = "CPU.NZCV.CSEL.001",
        .status = pass ? TEST_PASS : TEST_FAIL,
        .expected = 5,
        .actual = actual,
        .nzcv_before = nzcv_before,
        .nzcv_after = nzcv_after,
        .result_code = 0,
        .detail = "cmp 5,7 then csel lt",
    };
}

TestResult run_cpu_reg_preserve_001(void) {
    const uint32_t mismatch_mask = nce_diag_cpu_reg_preserve_probe();

    return (TestResult){
        .id = "CPU.REG.PRESERVE.001",
        .status = mismatch_mask == 0 ? TEST_PASS : TEST_FAIL,
        .expected = 0,
        .actual = mismatch_mask,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = 0,
        .detail = "x19-x28 across direct BL/RET",
    };
}
