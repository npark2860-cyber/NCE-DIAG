# NEXT_ACTION

Do not add new testcase IDs.

Validated witness implementation: `f27b3b41a36c320f8453050059ebbb8efaefda57`.
Validated CI run: `34702903483` PASS.
Executable artifact ID: `10300611444`.

Confirmed previous CINC boundary:

```text
00_ENTER completed
10_PRE_SEQUENCE completed
raw CINC sequence returned
20_POST_SEQUENCE SVC 0x27 entered
host terminated before SVC return
```

Next action:

1. Run `NCE-DIAG-CINC.nro` from artifact `10300611444` with config/persistence disabled.
2. Capture the Eden log from boot until normal exit or host-process termination.
3. Inspect the final `IMP008_REENTRY_STATE_PRE` SVC 0x27 line.
4. A witness checkpoint has `x19=4E434554xxxxxxxx` and `x20=4E434543xxxxxxxx`.
5. Decode directly with `python3 scripts/decode_eden_checkpoint.py <eden_log>` or use the known CINC tags below.

CINC witness values:

- test `CPU.NZCV.CINC.001`: `x19=4E4345541715C97C`
- `00_ENTER`: `x20=4E434543E1972DC0`
- `10_PRE_SEQUENCE`: `x20=4E434543B578118E`
- `20_POST_SEQUENCE`: `x20=4E434543F9A02A56`
- `30_RESULT_CAPTURED`: `x20=4E434543C3824CB1`
- `90_RESULT_RETURNED`: `x20=4E434543B4AFF18E`

Primary success criterion: CINC reaches `90_RESULT_RETURNED` and PASS/END.

If the host terminates again, the final witness PRE line is the authoritative guest boundary. Do not infer guest TEST_FAIL from a host-process termination and do not reintroduce the old formatter probes.

Only after the CINC-only path is runtime-stable should the full five-test suite be run unchanged.
