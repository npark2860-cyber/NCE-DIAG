#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include "nce_diag.h"
#include "nce_diag_raw_abi.h"

#define NCE_DIAG_VERSION "0.2.0"

typedef enum SelectionMode {
    SELECT_ALL = 0,
    SELECT_SINGLE = 1,
    SELECT_RANGE = 2,
} SelectionMode;

typedef struct RunSelection {
    SelectionMode mode;
    char test_id[64];
    size_t first_index;
    size_t last_index;
    bool persist;
    bool allow_config;
    bool selection_from_argv;
    const char *source;
} RunSelection;

static const NceDiagTestCase g_tests[] = {
    {"CPU.NZCV.CINC.001", run_cpu_nzcv_cinc_001},
    {"CPU.NZCV.CSEL.001", run_cpu_nzcv_csel_001},
    {"CPU.REG.PRESERVE.001", run_cpu_reg_preserve_001},
    {"IPC.SM.GET_SERVICE.001", run_ipc_sm_get_service_001},
    {"IPC.SVC21.REPEATED.001", run_ipc_svc21_repeated_001},
};

#define NCE_DIAG_TEST_COUNT (sizeof(g_tests) / sizeof(g_tests[0]))

static const char *test_status_name(TestStatus status) {
    switch (status) {
        case TEST_PASS: return "PASS";
        case TEST_FAIL: return "FAIL";
        case TEST_SKIP: return "SKIP";
        default:        return "UNKNOWN";
    }
}

static char *trim(char *text) {
    while (*text && isspace((unsigned char)*text)) {
        ++text;
    }

    char *end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

static bool parse_range(const char *text, size_t *first, size_t *last) {
    char *dash = strchr(text, '-');
    if (!dash || dash == text || dash[1] == '\0') {
        return false;
    }

    char left[16];
    const size_t left_len = (size_t)(dash - text);
    if (left_len >= sizeof(left)) {
        return false;
    }

    memcpy(left, text, left_len);
    left[left_len] = '\0';

    char *end_left = NULL;
    char *end_right = NULL;
    const unsigned long first_value = strtoul(left, &end_left, 10);
    const unsigned long last_value = strtoul(dash + 1, &end_right, 10);

    if (!end_left || *end_left != '\0' || !end_right || *end_right != '\0') {
        return false;
    }
    if (first_value == 0 || last_value == 0 || first_value > last_value) {
        return false;
    }
    if (last_value > NCE_DIAG_TEST_COUNT) {
        return false;
    }

    *first = (size_t)first_value - 1;
    *last = (size_t)last_value - 1;
    return true;
}

static bool find_test_index(const char *id, size_t *out_index) {
    for (size_t i = 0; i < NCE_DIAG_TEST_COUNT; ++i) {
        if (strcmp(g_tests[i].id, id) == 0) {
            *out_index = i;
            return true;
        }
    }
    return false;
}

static bool apply_selection_kv(RunSelection *selection, const char *key, const char *value) {
    if (strcmp(key, "run") == 0 && strcmp(value, "all") == 0) {
        selection->mode = SELECT_ALL;
        selection->first_index = 0;
        selection->last_index = NCE_DIAG_TEST_COUNT - 1;
        return true;
    }

    if (strcmp(key, "test") == 0) {
        if (strlen(value) >= sizeof(selection->test_id)) {
            return false;
        }
        strcpy(selection->test_id, value);
        selection->mode = SELECT_SINGLE;
        return true;
    }

    if (strcmp(key, "range") == 0) {
        size_t first = 0;
        size_t last = 0;
        if (!parse_range(value, &first, &last)) {
            return false;
        }
        selection->mode = SELECT_RANGE;
        selection->first_index = first;
        selection->last_index = last;
        return true;
    }

    if (strcmp(key, "persist") == 0) {
        if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0) {
            selection->persist = true;
            return true;
        }
        if (strcmp(value, "0") == 0 || strcmp(value, "false") == 0) {
            selection->persist = false;
            return true;
        }
        return false;
    }

    return false;
}

static bool load_selection_config(RunSelection *selection) {
    static const char *const paths[] = {
        "sdmc:/nce_diag.cfg",
        "nce_diag.cfg",
    };

    FILE *fp = NULL;
    const char *selected_path = NULL;

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        nce_diag_logf("STARTUP 30_CONFIG_PRE_READ path=%s", paths[i]);
        fp = fopen(paths[i], "r");
        if (fp) {
            selected_path = paths[i];
            nce_diag_logf("STARTUP 31_CONFIG_OPENED path=%s", paths[i]);
            break;
        }
        nce_diag_logf("STARTUP 31_CONFIG_NOT_FOUND path=%s", paths[i]);
    }

    if (!fp) {
        return false;
    }

    char line[160];
    while (fgets(line, sizeof(line), fp)) {
        char *item = trim(line);
        if (*item == '\0' || *item == '#') {
            continue;
        }

        char *equals = strchr(item, '=');
        if (!equals) {
            nce_diag_logf("CONFIG IGNORED line=%s", item);
            continue;
        }

        *equals = '\0';
        char *key = trim(item);
        char *value = trim(equals + 1);

        if (!apply_selection_kv(selection, key, value)) {
            nce_diag_logf("CONFIG INVALID key=%s value=%s", key, value);
        }
    }

    (void)fclose(fp);
    selection->source = selected_path;
    nce_diag_logf("STARTUP 32_CONFIG_DONE path=%s", selected_path);
    return true;
}

static void parse_arguments(int argc, char **argv, RunSelection *selection) {
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (strcmp(arg, "--all") == 0) {
            selection->mode = SELECT_ALL;
            selection->first_index = 0;
            selection->last_index = NCE_DIAG_TEST_COUNT - 1;
            selection->selection_from_argv = true;
            selection->source = "argv";
        } else if (strncmp(arg, "--test=", 7) == 0) {
            const char *id = arg + 7;
            if (strlen(id) < sizeof(selection->test_id)) {
                strcpy(selection->test_id, id);
                selection->mode = SELECT_SINGLE;
                selection->selection_from_argv = true;
                selection->source = "argv";
            }
        } else if (strncmp(arg, "--range=", 8) == 0) {
            size_t first = 0;
            size_t last = 0;
            if (parse_range(arg + 8, &first, &last)) {
                selection->mode = SELECT_RANGE;
                selection->first_index = first;
                selection->last_index = last;
                selection->selection_from_argv = true;
                selection->source = "argv";
            }
        } else if (strcmp(arg, "--persist") == 0) {
            selection->persist = true;
        } else if (strcmp(arg, "--debug-only") == 0) {
            selection->persist = false;
        } else if (strcmp(arg, "--no-config") == 0) {
            selection->allow_config = false;
        } else {
            size_t ignored_index = 0;
            if (find_test_index(arg, &ignored_index) && strlen(arg) < sizeof(selection->test_id)) {
                strcpy(selection->test_id, arg);
                selection->mode = SELECT_SINGLE;
                selection->selection_from_argv = true;
                selection->source = "argv";
            }
        }
    }
}

static bool resolve_selection(RunSelection *selection) {
    if (selection->mode == SELECT_SINGLE) {
        size_t index = 0;
        if (!find_test_index(selection->test_id, &index)) {
            nce_diag_logf("TEST_SELECTION ERROR unknown_test=%s", selection->test_id);
            return false;
        }
        selection->first_index = index;
        selection->last_index = index;
    }

    if (selection->first_index >= NCE_DIAG_TEST_COUNT ||
        selection->last_index >= NCE_DIAG_TEST_COUNT ||
        selection->first_index > selection->last_index) {
        nce_diag_logf("TEST_SELECTION ERROR invalid_range first=%zu last=%zu",
            selection->first_index + 1,
            selection->last_index + 1);
        return false;
    }

    return true;
}

static TestResult run_one_test(const NceDiagTestCase *test_case) {
    nce_diag_logf("BEGIN %s", test_case->id);
    nce_diag_checkpoint(test_case->id, "00_ENTER");

    TestResult result = test_case->run();

    nce_diag_checkpoint(test_case->id, "90_RESULT_RETURNED");
    nce_diag_logf(
        "%s %s expected=%" PRIu64 " actual=%" PRIu64 " rc=0x%08" PRIX32,
        test_status_name(result.status),
        test_case->id,
        result.expected,
        result.actual,
        result.result_code);
    nce_diag_logf("END %s", test_case->id);
    return result;
}

static bool write_json_to_path(
    const char *path,
    const TestResult *tests,
    size_t count,
    unsigned pass_count,
    unsigned fail_count,
    unsigned skip_count,
    size_t first_index,
    size_t last_index) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return false;
    }

    bool ok = true;
    if (fprintf(fp, "{\n") < 0) ok = false;
    if (fprintf(fp, "  \"suite\": \"NCE-DIAG\",\n") < 0) ok = false;
    if (fprintf(fp, "  \"version\": \"%s\",\n", NCE_DIAG_VERSION) < 0) ok = false;
    if (fprintf(fp, "  \"run_id\": %u,\n", nce_diag_get_run_id()) < 0) ok = false;
    if (fprintf(fp, "  \"selected_first\": %zu,\n", first_index + 1) < 0) ok = false;
    if (fprintf(fp, "  \"selected_last\": %zu,\n", last_index + 1) < 0) ok = false;
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
    unsigned skip_count,
    size_t first_index,
    size_t last_index) {
    static const char *const paths[] = {
        "sdmc:/nce_diag_result.json",
        "nce_diag_result.json",
    };

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        nce_diag_logf("RESULT_FILE 10_PRE_WRITE path=%s", paths[i]);
        if (write_json_to_path(
                paths[i],
                tests,
                count,
                pass_count,
                fail_count,
                skip_count,
                first_index,
                last_index)) {
            nce_diag_logf("RESULT_FILE 20_POST_WRITE path=%s", paths[i]);
            return paths[i];
        }
        nce_diag_logf("RESULT_FILE 11_WRITE_FAILED path=%s", paths[i]);
    }

    return NULL;
}

int main(int argc, char **argv) {
    const uint32_t run_id = (uint32_t)(armGetSystemTick() % UINT64_C(1000000));
    nce_diag_set_run_id(run_id);

    nce_diag_logf("START version=%s", NCE_DIAG_VERSION);
    nce_diag_logf("STARTUP 00_MAIN_ENTER");
    nce_diag_logf("STARTUP 10_RUNTIME_READY");
    nce_diag_logf("STARTUP 20_HARNESS_INIT");

    RunSelection selection = {
        .mode = SELECT_ALL,
        .first_index = 0,
        .last_index = NCE_DIAG_TEST_COUNT - 1,
        .persist = false,
        .allow_config = true,
        .selection_from_argv = false,
        .source = "default",
    };

    parse_arguments(argc, argv, &selection);
    if (selection.allow_config && !selection.selection_from_argv) {
        (void)load_selection_config(&selection);
    }

    if (!resolve_selection(&selection)) {
        nce_diag_logf("STOP reason=selection_error");
        return 2;
    }

    if (selection.persist) {
        nce_diag_logf("PERSISTENCE 00_PRE_OPEN");
        (void)nce_diag_persistence_enable();
    }

    const char *mode_name =
        selection.mode == SELECT_SINGLE ? "single" :
        selection.mode == SELECT_RANGE ? "range" : "all";

    nce_diag_logf(
        "TEST_SELECTION mode=%s first=%zu last=%zu persist=%u source=%s",
        mode_name,
        selection.first_index + 1,
        selection.last_index + 1,
        selection.persist ? 1u : 0u,
        selection.source ? selection.source : "default");

    TestResult results[NCE_DIAG_TEST_COUNT] = {0};
    size_t result_count = 0;
    unsigned pass_count = 0;
    unsigned fail_count = 0;
    unsigned skip_count = 0;

    nce_diag_logf("SUITE BEGIN selected=%zu-%zu",
        selection.first_index + 1,
        selection.last_index + 1);

    for (size_t i = selection.first_index; i <= selection.last_index; ++i) {
        TestResult result = run_one_test(&g_tests[i]);
        results[result_count++] = result;

        if (result.status == TEST_PASS) ++pass_count;
        else if (result.status == TEST_FAIL) ++fail_count;
        else ++skip_count;
    }

    nce_diag_logf("SUITE SUMMARY pass=%u fail=%u skip=%u", pass_count, fail_count, skip_count);
    nce_diag_logf("SUITE END");

    const char *result_path = write_result_file(
        results,
        result_count,
        pass_count,
        fail_count,
        skip_count,
        selection.first_index,
        selection.last_index);

    if (result_path) {
        nce_diag_logf("RESULT WRITTEN path=%s", result_path);
    } else {
        nce_diag_logf("RESULT WRITE_FAILED");
    }

    nce_diag_logf("STOP version=%s pass=%u fail=%u", NCE_DIAG_VERSION, pass_count, fail_count);
    nce_diag_persistence_disable();

    return (fail_count == 0 && result_path != NULL) ? 0 : 1;
}
