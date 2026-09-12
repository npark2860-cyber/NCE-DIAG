#include <stdbool.h>
#include <stdint.h>

#include <switch.h>

#include "nce_diag.h"

static Result close_if_valid(Handle handle) {
    if (handle == INVALID_HANDLE) {
        return 0;
    }
    return svcCloseHandle(handle);
}

TestResult run_ipc_sm_get_service_001(void) {
    Handle handle = INVALID_HANDLE;
    const Result get_rc = smGetServiceOriginal(&handle, smEncodeName("fsp-srv"));
    Result final_rc = get_rc;
    bool pass = R_SUCCEEDED(get_rc) && handle != INVALID_HANDLE;

    if (pass) {
        const Result close_rc = close_if_valid(handle);
        if (R_FAILED(close_rc)) {
            pass = false;
            final_rc = close_rc;
        }
    }

    return (TestResult){
        .id = "IPC.SM.GET_SERVICE.001",
        .status = pass ? TEST_PASS : TEST_FAIL,
        .expected = 1,
        .actual = pass ? 1 : 0,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = (uint32_t)final_rc,
        .detail = "sm:GetServiceOriginal(fsp-srv) moved handle",
    };
}

TestResult run_ipc_svc21_repeated_001(void) {
    enum { ITERATIONS = 16 };
    uint32_t completed = 0;
    Result first_error = 0;

    Service *sm_session = smGetServiceSession();
    if (!serviceIsActive(sm_session)) {
        first_error = UINT32_C(0xFFFFFFFF);
    } else {
        for (uint32_t i = 0; i < ITERATIONS; ++i) {
            Handle handle = INVALID_HANDLE;
            const Result get_rc = smGetServiceOriginal(&handle, smEncodeName("fsp-srv"));
            if (R_FAILED(get_rc) || handle == INVALID_HANDLE) {
                first_error = R_FAILED(get_rc) ? get_rc : UINT32_C(0xFFFFFFFE);
                break;
            }

            const Result close_rc = svcCloseHandle(handle);
            if (R_FAILED(close_rc)) {
                first_error = close_rc;
                break;
            }

            ++completed;
        }
    }

    return (TestResult){
        .id = "IPC.SVC21.REPEATED.001",
        .status = completed == ITERATIONS ? TEST_PASS : TEST_FAIL,
        .expected = ITERATIONS,
        .actual = completed,
        .nzcv_before = 0,
        .nzcv_after = 0,
        .result_code = (uint32_t)first_error,
        .detail = "16 CMIF GetService requests on one SM session",
    };
}
