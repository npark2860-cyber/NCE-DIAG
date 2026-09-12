#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <switch.h>

#include "nce_diag.h"

#define NCE_DIAG_RC_SM_NOT_READY UINT32_C(0xFFF00001)

static Result sm_get_service_checkpointed(
    const char *test_id,
    uint32_t iteration,
    bool include_iteration,
    Handle *out_handle) {
    if (!nce_diag_sm_ready()) {
        return NCE_DIAG_RC_SM_NOT_READY;
    }

    Service *sm_session = smGetServiceSession();
    if (!serviceIsActive(sm_session)) {
        return NCE_DIAG_RC_SM_NOT_READY;
    }

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "10_HANDLE_READY", "iter=%02u", iteration);
    } else {
        nce_diag_checkpoint(test_id, "10_HANDLE_READY");
    }

    const SmServiceName name = smEncodeName("fsp-srv");
    const SfBufferAttrs buffer_attrs = {0};
    const SfBuffer buffers[8] = {0};

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "15_PRE_REQUEST_BUILD", "iter=%02u", iteration);
    } else {
        nce_diag_checkpoint(test_id, "15_PRE_REQUEST_BUILD");
    }

    void *request_data = serviceMakeRequest(
        sm_session,
        1,
        0,
        (u32)sizeof(name),
        false,
        buffer_attrs,
        buffers,
        0,
        NULL,
        0,
        NULL);
    memcpy(request_data, &name, sizeof(name));

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "20_REQUEST_BUILT", "iter=%02u", iteration);
        nce_diag_checkpointf(test_id, "30_PRE_SVC", "iter=%02u", iteration);
    } else {
        nce_diag_checkpoint(test_id, "20_REQUEST_BUILT");
        nce_diag_checkpoint(test_id, "30_PRE_SVC");
    }

    Result rc = svcSendSyncRequest(sm_session->session);

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "40_POST_SVC", "iter=%02u rc=0x%08X", iteration, (uint32_t)rc);
    } else {
        nce_diag_checkpointf(test_id, "40_POST_SVC", "rc=0x%08X", (uint32_t)rc);
    }

    if (R_FAILED(rc)) {
        return rc;
    }

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "45_PRE_RESULT_PARSE", "iter=%02u", iteration);
    } else {
        nce_diag_checkpoint(test_id, "45_PRE_RESULT_PARSE");
    }

    const SfOutHandleAttrs out_handle_attrs = {
        .attr0 = SfOutHandleAttr_HipcMove,
    };
    rc = serviceParseResponse(
        sm_session,
        0,
        NULL,
        0,
        NULL,
        out_handle_attrs,
        out_handle);

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "50_RESULT_PARSED", "iter=%02u rc=0x%08X", iteration, (uint32_t)rc);
    } else {
        nce_diag_checkpointf(test_id, "50_RESULT_PARSED", "rc=0x%08X", (uint32_t)rc);
    }

    return rc;
}

static Result close_handle_checkpointed(
    const char *test_id,
    uint32_t iteration,
    bool include_iteration,
    Handle handle) {
    if (handle == INVALID_HANDLE) {
        return 0;
    }

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "55_PRE_CLOSE", "iter=%02u", iteration);
    } else {
        nce_diag_checkpoint(test_id, "55_PRE_CLOSE");
    }

    const Result rc = svcCloseHandle(handle);

    if (include_iteration) {
        nce_diag_checkpointf(test_id, "56_POST_CLOSE", "iter=%02u rc=0x%08X", iteration, (uint32_t)rc);
    } else {
        nce_diag_checkpointf(test_id, "56_POST_CLOSE", "rc=0x%08X", (uint32_t)rc);
    }

    return rc;
}

static TestResult sm_not_ready_result(const char *id, uint64_t expected, const char *detail) {
    nce_diag_checkpoint(id, "06_SM_NOT_READY");
    return (TestResult){
        .id = id,
        .status = TEST_FAIL,
        .expected = expected,
        .actual = 0,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = NCE_DIAG_RC_SM_NOT_READY,
        .detail = detail,
    };
}

TestResult run_ipc_sm_get_service_001(void) {
    static const char *const id = "IPC.SM.GET_SERVICE.001";
    nce_diag_checkpoint(id, "05_SM_READY_CHECK");
    if (!nce_diag_sm_ready()) {
        return sm_not_ready_result(id, 1, "minimal runtime SM initialization failed");
    }

    Handle handle = INVALID_HANDLE;
    Result final_rc = sm_get_service_checkpointed(id, 0, false, &handle);
    bool pass = R_SUCCEEDED(final_rc) && handle != INVALID_HANDLE;

    if (pass) {
        const Result close_rc = close_handle_checkpointed(id, 0, false, handle);
        if (R_FAILED(close_rc)) {
            pass = false;
            final_rc = close_rc;
        }
    }

    nce_diag_checkpoint(id, "60_DONE");

    return (TestResult){
        .id = id,
        .status = pass ? TEST_PASS : TEST_FAIL,
        .expected = 1,
        .actual = pass ? 1 : 0,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = (uint32_t)final_rc,
        .detail = "SM command 1 / SVC 0x21 / moved fsp-srv handle",
    };
}

TestResult run_ipc_svc21_repeated_001(void) {
    static const char *const id = "IPC.SVC21.REPEATED.001";
    enum { ITERATIONS = 16 };
    nce_diag_checkpoint(id, "05_SM_READY_CHECK");
    if (!nce_diag_sm_ready()) {
        return sm_not_ready_result(id, ITERATIONS, "minimal runtime SM initialization failed");
    }

    uint32_t completed = 0;
    Result first_error = 0;

    for (uint32_t i = 0; i < ITERATIONS; ++i) {
        Handle handle = INVALID_HANDLE;
        const uint32_t iteration = i + 1;
        const Result get_rc = sm_get_service_checkpointed(id, iteration, true, &handle);
        if (R_FAILED(get_rc) || handle == INVALID_HANDLE) {
            first_error = R_FAILED(get_rc) ? get_rc : UINT32_C(0xFFFFFFFE);
            break;
        }

        const Result close_rc = close_handle_checkpointed(id, iteration, true, handle);
        if (R_FAILED(close_rc)) {
            first_error = close_rc;
            break;
        }

        ++completed;
        nce_diag_checkpointf(id, "60_ITER_DONE", "iter=%02u", iteration);
    }

    nce_diag_checkpointf(id, "70_LOOP_DONE", "completed=%u", completed);

    return (TestResult){
        .id = id,
        .status = completed == ITERATIONS ? TEST_PASS : TEST_FAIL,
        .expected = ITERATIONS,
        .actual = completed,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = (uint32_t)first_error,
        .detail = "16 explicit CMIF GetService requests on one SM session",
    };
}
