# NEXT_ACTION

Do not add new testcase IDs.

Validated implementation commit: `446d0becca38fa6c8f64e83e7cf7bc1541a8be83`.
Validated CI run: `34698719211` PASS.
Executable artifact ID: `10299149605`.

1. Run `NCE-DIAG-CINC.nro` from the validated executable artifact with config/persistence disabled.
2. Capture the Eden log from boot until normal exit or host-process termination.
3. Read the last `[NCE-DIAG]` line directly. The final `CKPT <test-id> <checkpoint-id>` is the crash boundary; no guest FAIL is inferred from a host crash.
4. Primary success criterion: `CPU.NZCV.CINC.001` reaches `90_RESULT_RETURNED` and emits PASS/END without entering the former svfprintf probe path.
5. If the host still terminates earlier, use the last named checkpoint as the next single-variable boundary. Do not reintroduce formatter dependency probes into the stable harness.
6. After CINC-only is runtime-stable, run the full five-test suite unchanged.
