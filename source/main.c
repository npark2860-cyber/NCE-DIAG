#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <switch.h>

#include "nce_diag.h"
#include "nce_diag_raw_abi.h"

#define NCE_DIAG_VERSION "0.1.0"
#define NCE_DIAG_TEST_COUNT 5u

static const char *test_status_name(TestStatus status) {
    switch (status) {
        case TEST_PASS: return "PASS";
        case TEST_FAIL: return "FAIL";
        case TEST_SKIP: return "SKIP";
        default:        return "UNKNOWN";
    }
}

static void print_failure(const TestResult *result) {
    printf("\nFAIL %s\n", result->id);
    printf("detail=%s\n", result->detail);
    printf("expected=%" PRIu64 "\n", result->expected);
    printf("actual=%" PRIu64 "\n", result->actual);
    printf("result_code=0x%08" PRIX32 "\n", result->result_code);
    if (result->nzcv_before || result->nzcv_after) {
        printf("nzcv_before=0x%08" PRIX64 "\n", result->nzcv_before);
        printf("nzcv_after=0x%08" PRIX64 "\n", result->nzcv_after);
    }
}

static bool write_json_to_path(
    const char *path,
    const TestResult *tests,
    size_t count,
    unsigned pass_count,
    unsigned fail_count,
    unsigned skip_count) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return false;
    }

    bool ok = true;
    if (fprintf(fp, "{\n") < 0) ok = false;
    if (fprintf(fp, "  \"suite\": \"NCE-DIAG\",\n") < 0) ok = false;
    if (fprintf(fp, "  \"version\": \"%s\",\n", NCE_DIAG_VERSION) < 0) ok = false;
    if (fprintf(fp, "  \"pass\": %u,\n", pass_count) < 0) ok = false;
    if (fprintf(fp, "  \"fail\": %u,\n", fail_count) < 0) ok = false;
    if (fprintf(fp, "  \"skip\": %u,\n", skip_count) < 0) ok = false;
    if (fprintf(fp, "  \"tests\": [\n") < 0) ok = false;

    for (size_t i = 0; i < count; ++i) {
        const TestResult *t = &tests[i];
        if (fprintf(fp,
                "    {\"id\":\"%s\",\"status\":\"%s\",\"expected\":%" PRIu64
                ",\"actual\":%" PRIu64 ",\"result_code\":\"0x%08" PRIX32
                "\",\"nzcv_before\":\"0x%08" PRIX64 "\",\"nzcv_after\":\"0x%08" PRIX64
                "\",\"detail\":\"%s\"}%s\n",
                t->id,
                test_status_name(t->status),
                t->expected,
                t->actual,
                t->result_code,
                t->nzcv_before,
                t->nzcv_after,
                t->detail,
                (i + 1 == count) ? "" : ",") < 0) {
            ok = false;
        }
    }

    if (fprintf(fp, "  ]\n}\n") < 0) ok = false;
    if (fflush(fp) != 0) ok = false;
    if (fclose(fp) != 0) ok = false;
    return ok;
}

static const char *write_result_file(
    const TestResult *tests,
    size_t count,
    unsigned pass_count,
    unsigned fail_count,
    unsigned skip_count) {
    static const char *const paths[] = {
        "sdmc:/nce_diag_result.json",
        "nce_diag_result.json",
    };

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        if (write_json_to_path(paths[i], tests, count, pass_count, fail_count, skip_count)) {
            return paths[i];
        }
    }

    return NULL;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    consoleInit(NULL);

    printf("NCE-DIAG %s\n\n", NCE_DIAG_VERSION);

    TestResult tests[NCE_DIAG_TEST_COUNT];
    tests[0] = run_cpu_nzcv_cinc_001();
    tests[1] = run_cpu_nzcv_csel_001();
    tests[2] = run_cpu_reg_preserve_001();
    tests[3] = run_ipc_sm_get_service_001();
    tests[4] = run_ipc_svc21_repeated_001();

    unsigned pass_count = 0;
    unsigned fail_count = 0;
    unsigned skip_count = 0;

    for (size_t i = 0; i < NCE_DIAG_TEST_COUNT; ++i) {
        const TestResult *result = &tests[i];
        printf("%-32s %s\n", result->id, test_status_name(result->status));
        if (result->status == TEST_PASS) ++pass_count;
        else if (result->status == TEST_FAIL) ++fail_count;
        else ++skip_count;
    }

    printf("\n-----------------------------------\n");
    printf("PASS %u\n", pass_count);
    printf("FAIL %u\n", fail_count);
    printf("SKIP %u\n", skip_count);

    for (size_t i = 0; i < NCE_DIAG_TEST_COUNT; ++i) {
        if (tests[i].status == TEST_FAIL) {
            print_failure(&tests[i]);
        }
    }

    const char *result_path = write_result_file(
        tests, NCE_DIAG_TEST_COUNT, pass_count, fail_count, skip_count);

    if (result_path) {
        printf("\nResult written:\n%s\n", result_path);
    } else {
        printf("\nResult write FAILED\n");
    }

    consoleUpdate(NULL);
    svcSleepThread(UINT64_C(1000000000));
    consoleExit(NULL);

    return (fail_count == 0 && result_path != NULL) ? 0 : 1;
}
