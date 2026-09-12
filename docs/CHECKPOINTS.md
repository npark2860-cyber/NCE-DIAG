# Crash-resilient checkpoint format

NCE-DIAG 0.2 treats the emulator process itself as a possible failure boundary.

Primary output is a direct `svcOutputDebugString` call (SVC 0x27). Checkpoints are emitted before dangerous guest operations and again after they return.

Format:

```text
[NCE-DIAG][RUN=123456] BEGIN CPU.NZCV.CINC.001
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 10_PRE_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 20_POST_SEQUENCE
[NCE-DIAG][RUN=123456] CKPT CPU.NZCV.CINC.001 30_RESULT_CAPTURED
[NCE-DIAG][RUN=123456] PASS CPU.NZCV.CINC.001 expected=2 actual=2 rc=0x00000000
[NCE-DIAG][RUN=123456] END CPU.NZCV.CINC.001
```

Internal checkpoint logging is debug-output only. It never performs filesystem persistence. This prevents persistent-mode file IPC from overwriting the TLS IPC request buffer between `REQUEST_BUILT` and `svcSendSyncRequest`.

Persistent mode journals safe lifecycle records (`BEGIN`, `PASS`/`FAIL`, `END`) and flushes each record. It is optional and disabled by default.

## Normal PASS sample

```text
[NCE-DIAG][RUN=123456] START version=0.2.0
[NCE-DIAG][RUN=123456] STARTUP 00_MAIN_ENTER
[NCE-DIAG][RUN=123456] STARTUP 10_RUNTIME_READY
[NCE-DIAG][RUN=123456] STARTUP 20_HARNESS_INIT
[NCE-DIAG][RUN=123456] TEST_SELECTION mode=single first=1 last=1 persist=0
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

## Expected crash-boundary sample

```text
[NCE-DIAG][RUN=123456] BEGIN IPC.SVC21.REPEATED.001
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 20_REQUEST_BUILT iter=07
[NCE-DIAG][RUN=123456] CKPT IPC.SVC21.REPEATED.001 30_PRE_SVC iter=07
```

If the Eden host process disappears here, classify the boundary as:

```text
HOST_PROCESS_CRASH_AFTER_30_PRE_SVC
IPC.SVC21.REPEATED.001 iter=07
boundary: 30_PRE_SVC -> 40_POST_SVC
```

Do not record this as a guest test FAIL; the guest never returned from the operation.
