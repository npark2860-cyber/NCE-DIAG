# Crash-resilient checkpoint format

NCE-DIAG 0.2.1 treats the emulator process itself as a possible failure boundary.

Primary output is direct `svcOutputDebugString` (SVC 0x27).

The critical debug path uses a project-owned fixed-buffer formatter. `nce_diag_logf`, `nce_diag_checkpoint`, and `nce_diag_checkpointf` do not call `snprintf`, `vsnprintf`, `_svfprintf_r`, locale code, or heap allocation. This prevents the diagnostic channel from becoming the architectural workload being diagnosed.

## Pre-main boot markers

The custom minimal `__appInit` emits static markers before normal harness code exists:

```text
[NCE-DIAG][BOOT] 00_APP_INIT_ENTER
[NCE-DIAG][BOOT] 10_PRE_SM_INIT
[NCE-DIAG][BOOT] 20_POST_SM_INIT
[NCE-DIAG][BOOT] 30_APP_INIT_DONE
```

If SM initialization fails, `21_SM_INIT_FAILED` replaces `20_POST_SM_INIT`.

There are no constructor/main-wrapper/svfprintf probe markers in the stable diagnostic build.

## Test markers

Format:

```text
[NCE-DIAG][RUN=123456] BEGIN CPU.NZCV.CINC.001
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 00_ENTER
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 10_PRE_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 20_POST_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 30_RESULT_CAPTURED
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 90_RESULT_RETURNED
[NCE-DIAG][RUN=123456] PASS CPU.NZCV.CINC.001 expected=2 actual=2 rc=0x00000000
[NCE-DIAG][RUN=123456] END CPU.NZCV.CINC.001
```

CKPT logging is debug-output only and never performs filesystem persistence.

## Host crash interpretation

Example:

```text
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 20_REQUEST_BUILT iter=07
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 30_PRE_SVC iter=07
```

If the host process disappears before `40_POST_SVC`, the authoritative boundary is:

```text
30_PRE_SVC -> 40_POST_SVC
```

A host-process exit is not classified as guest `TEST_FAIL`.

## Optional FS markers

Config/persistence explicitly enters a separately observable path:

```text
OPTIONAL_FS 10_PRE_FS_INIT
OPTIONAL_FS 20_POST_FS_INIT
OPTIONAL_FS 30_PRE_SD_MOUNT
OPTIONAL_FS 40_POST_SD_MOUNT
```

Default debug-only execution does not enter this path.
