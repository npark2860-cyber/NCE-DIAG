# NEXT_ACTION

Do not add new testcase IDs.

1. Build and validate the libc-independent checkpoint commit in CI.
2. Run the resulting `NCE-DIAG-CINC.nro` with config/persistence disabled.
3. Capture the Eden log from boot until normal exit or host-process termination.
4. Read the last `[NCE-DIAG]` line directly. The final `CKPT <test-id> <checkpoint-id>` is the crash boundary; no guest FAIL is inferred from a host crash.
5. Primary success criterion: `CPU.NZCV.CINC.001` reaches `90_RESULT_RETURNED` and emits PASS/END without entering the former svfprintf probe path.
6. If the host still terminates earlier, use the last named checkpoint as the next single-variable boundary. Do not reintroduce formatter dependency probes into the stable harness.
7. After CINC-only is runtime-stable, run the full five-test suite unchanged.
