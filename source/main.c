#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include <switch.h>

#include "nce_diag.h"

#define NCE_DIAG_VERSION "0.1.0"

static const char *test_status_name(TestStatus status) {
    switch (status) {
        case TEST_PASS: return "PASS";
        case TEST_FAIL: return "FAIL";
        case TEST_SKIP: return "SKIP";
        default:        return "UNKNOWN";
    }
}

TestResult run_cpu_nzcv_cinc_001(void) {
    static const char *const id = "CPU.NZCV.CINC.001";
    static const char *const detail = "cmp w16,#0 -> stp w17,w18 -> cinc ne";
    const uint64_t expected_nzcv = UINT64_C(0x60000000);
    const uint64_t expected_result = UINT64_C(2);

    uint32_t scratch[8] __attribute__((aligned(16))) = {0};
    uint64_t nzcv_before = 0;
    uint64_t nzcv_after = 0;

    const uint32_t actual = nce_diag_cpu_nzcv_cinc_probe(
        0, scratch, &nzcv_before, &nzcv_after);

    const bool pass =
        actual == expected_result &&
        nzcv_before == expected_nzcv &&
        nzcv_after == expected_nzcv;

    TestResult result = {
        .id = id,
        .status = pass ? TEST_PASS : TEST_FAIL,
        .expected = expected_result,
        .actual = actual,
        .nzcv_before = nzcv_before,
        .nzcv_after = nzcv_after,
        .detail = detail,
    };
    return result;
}

static void print_failure(const TestResult *result) {
    printf("\nFAIL %s\n", result->id);
    printf("sequence=%s\n", result->detail);
    printf("input.w16=0\n");
    printf("expected_result=%" PRIu64 "\n", result->expected);
    printf("actual_result=%" PRIu64 "\n", result->actual);
    printf("NZCV_expected=0x%08" PRIX64 "\n", UINT64_C(0x60000000));
    printf("NZCV_before=0x%08" PRIX64 "\n", result->nzcv_before);
    printf("NZCV_after=0x%08" PRIX64 "\n", result->nzcv_after);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    PadState pad;
    padInitializeDefault(&pad);

    printf("NCE-DIAG %s\n\n", NCE_DIAG_VERSION);

    const TestResult result = run_cpu_nzcv_cinc_001();
    printf("%-32s %s\n", result.id, test_status_name(result.status));

    const unsigned pass_count = result.status == TEST_PASS ? 1U : 0U;
    const unsigned fail_count = result.status == TEST_FAIL ? 1U : 0U;
    const unsigned skip_count = result.status == TEST_SKIP ? 1U : 0U;

    printf("\n-----------------------------------\n");
    printf("PASS: %u\n", pass_count);
    printf("FAIL: %u\n", fail_count);
    printf("SKIP: %u\n", skip_count);

    if (result.status == TEST_FAIL) {
        print_failure(&result);
    }

    printf("\nPress + to exit.\n");

    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) {
            break;
        }
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return fail_count == 0 ? 0 : 1;
}
