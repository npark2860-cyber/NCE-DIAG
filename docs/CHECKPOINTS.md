# Crash-resilient checkpoint format

NCE-DIAG 0.2.1 treats the emulator process itself as a possible failure boundary.

Primary output is direct `svcOutputDebugString` (SVC 0x27).

## Pre-main boot markers

The custom minimal `__appInit` emits markers before normal harness code exists:

```text
[NCE-DIAG][BOOT] 00_APP_INIT_ENTER
[NCE-DIAG][BOOT] 10_PRE_SM_INIT
[NCE-DIAG][BOOT] 20_POST_SM_INIT
[NCE-DIAG][BOOT] 30_APP_INIT_DONE
```

If SM initialization fails, `21_SM_INIT_FAILED` replaces `20_POST_SM_INIT`.

These markers have no run ID because they execute before `main()` chooses one.

## Test markers

Format:

```text
[NCE-DIAG][RUN=123456] BEGIN CPU.NZCV.CINC.001
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 10_PRE_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 20_POST_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 30_RESULT_CAPTURED
[NCE-DIAG][RUN=123456] PASS CPU.NZCV.CINC.001 expected=2 actual=2 rc=0x00000000
[NCE-DIAG][RUN=123456] END CPU.NZCV.CINC.001
```

Internal CKPT logging is debug-output only. It never performs filesystem persistence.

## Normal PASS sample

```text
[NCE-DIAG][BOOT] 00_APP_INIT_ENTER
[NCE-DIAG][BOOT] 10_PRE_SM_INIT
[NCE-DIAG][BOOT] 20_POST_SM_INIT
[NCE-DIAG][BOOT] 30_APP_INIT_DONE
[NCE-DIAG][RUN=123456] START version=0.2.1
[NCE-DIAG][RUN=123456] STARTUP 00_MAIN_ENTER
[NCE-DIAG][RUN=123456] TEST_SELECTION mode=range first=1 last=1 persist=0 source=compile-default
[NCE-DIAG][RUN=123456] BEGIN CPU.NZCV.CINC.001
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 00_ENTER
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 10_PRE_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 20_POST_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 30_RESULT_CAPTURED
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 90_RESULT_RETURNED
[NCE-DIAG][RUN=123456] PASS CPU.NZCV.CINC.001 expected=2 actual=2 rc=0x00000000
[NCE-DIAG][RUN=123456] END CPU.NZCV.CINC.001
```

## Normal FAIL sample

```text
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 90_RESULT_RETURNED
[NCE-DIAG][RUN=123456] FAIL CPU.NZCV.CINC.001 expected=2 actual=3 rc=0x00000000
[NCE-DIAG][RUN=123456] END CPU.NZCV.CINC.001
```

## Host crash sample

```text
[NCE-DIAG][RUN=123456] BEGIN IPC.SVC21.REPEATED.001
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 20_REQUEST_BUILT iter=07
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 30_PRE_SVC iter=07
```

If the host disappears here:

```text
HOST_PROCESS_CRASH_AFTER_30_PRE_SVC
boundary: 30_PRE_SVC -> 40_POST_SVC
```

Do not classify a host exit as guest TEST_FAIL.

## Optional FS markers

Config/persistence explicitly enters a separately observable path:

```text
OPTIONAL_FS 10_PRE_FS_INIT
OPTIONAL_FS 20_POST_FS_INIT
OPTIONAL_FS 30_PRE_SD_MOUNT
OPTIONAL_FS 40_POST_SD_MOUNT
```

Default debug-only execution does not enter this path.
