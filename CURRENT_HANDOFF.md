# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent internal guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Confirmed runtime evidence

The pre-witness CINC-only build was run on Eden validation build `exp/imp-008e-windows-nce-rasterizer-recovery-runtime-58a7b77273`.

Matching `NCE-DIAG-CINC.elf` symbolization established the following sequence:

- seven normal lifecycle SVC 0x27 calls completed through `BEGIN CPU.NZCV.CINC.001`
- checkpoint `00_ENTER` completed
- checkpoint `10_PRE_SEQUENCE` completed
- `nce_diag_raw_cpu_nzcv_cinc_001` executed and returned
- Eden entered SVC 0x27 for the next checkpoint, `20_POST_SEQUENCE`
- the final record was RunThread PRE `n=30`; no matching RunThread RETURN was recorded

Therefore the strongest supported boundary is:

```text
CPU.NZCV.CINC.001
10_PRE_SEQUENCE completed
-> raw CINC sequence executed and returned
-> 20_POST_SEQUENCE debug SVC entered
-> host process terminated before that SVC returned
```

This is not a guest `TEST_FAIL`. The run did not reach `30_RESULT_CAPTURED` or `90_RESULT_RETURNED`.

The run also exposed an observability gap: Eden records SVC 0x27 register state but does not include the `svcOutputDebugString` text payload in this log mode. Repeated checkpoints therefore needed ELF/control-flow reconstruction.

## Current implementation

- strong project-owned `__appInit` / `__appExit`
- startup initializes SM only
- no default applet/HID/time/FS/SDMC/console path before main
- FS remains lazy and opt-in
- full-suite and CINC-only executable NROs
- stable testcase IDs unchanged
- run-id bootstrap does not read `CNTPCT_EL0`
- critical diagnostic formatting is project-owned and does not call `snprintf`, `vsnprintf`, or `_svfprintf_r`
- temporary constructor/main/svfprintf probes remain removed
- all project-owned SVC 0x27 output is routed through `nce_diag_output_debug_string`
- a `CKPT <test> <checkpoint>` line is parsed before the SVC and encoded into temporary x19/x20 witness tags
- x19 test tag prefix: `0x4E43455400000000` (`NCET`)
- x20 checkpoint tag prefix: `0x4E43454300000000` (`NCEC`)
- low 32 bits are FNV-1a hashes of the test/checkpoint strings
- original x19/x20 are restored after SVC return
- `scripts/decode_eden_checkpoint.py` decodes witness tags directly from Eden `IMP008_REENTRY_STATE_PRE` lines

For `CPU.NZCV.CINC.001` the expected test witness is:

`x19=4E4345541715C97C`

Relevant checkpoint witnesses are:

- `00_ENTER` -> `x20=4E434543E1972DC0`
- `10_PRE_SEQUENCE` -> `x20=4E434543B578118E`
- `20_POST_SEQUENCE` -> `x20=4E434543F9A02A56`
- `30_RESULT_CAPTURED` -> `x20=4E434543C3824CB1`
- `90_RESULT_RETURNED` -> `x20=4E434543B4AFF18E`

## Validated witness build

Validated implementation HEAD: `f27b3b41a36c320f8453050059ebbb8efaefda57`.

GitHub Actions run `34702903483`: PASS.

Validation includes:

- full-suite and CINC-only NRO build
- libc-independent checkpoint path
- x19/x20 witness assembly shape and register restoration
- witness decoder self-test
- auxiliary raw instruction-shape validation
- executable/debug-symbol/raw-microtest artifact upload

Executable artifact: `NCE-DIAG-0.2.1-executables`, artifact ID `10300611444`, SHA-256 digest `61457efa1a474405b7f57cedf2fab0dd78c3e8f12adc41694e4b565f911d0efd`.

## Runtime status

The next required runtime is the witness-enabled `NCE-DIAG-CINC.nro` from artifact `10300611444` with config/persistence disabled.

If Eden terminates again inside SVC 0x27, the final PRE line itself is now sufficient to identify the test/checkpoint without reading the debug-string payload or symbolizing the ELF.
