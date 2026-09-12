# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

## 0.2.1 crash-observability runtime

Primary artifact: `NCE-DIAG.nro`.

`NCE-DIAG-CINC.nro` runs registry index 1 only (`CPU.NZCV.CINC.001`) for isolated runtime validation.

The primary diagnostic channel is direct `svcOutputDebugString` (SVC 0x27). GUI rendering is not required.

### Minimal startup

The project overrides libnx's weak `__appInit` / `__appExit`.

Default startup is:

```text
libnx core runtime
-> static BOOT marker
-> SM initialize only
-> static BOOT marker
-> main
-> test harness
```

Default startup does not initialize applet, HID, time, FS, SDMC, or a console framebuffer.

### Crash-safe diagnostic output

The stable debug path does not use libc formatted I/O. `nce_diag_logf`, `nce_diag_checkpoint`, and `nce_diag_checkpointf` format into fixed local buffers with project-owned integer/string conversion and then call `svcOutputDebugString` directly.

This keeps the diagnostic reporter independent from the newlib `snprintf/vsnprintf/_svfprintf_r` path that was itself reached by earlier probe builds.

Temporary constructor/main-wrapper/svfprintf dependency probes are not part of the stable executable.

### Test lifecycle

Every testcase follows:

```text
BEGIN
CKPT before dangerous operation
CKPT after dangerous operation
PASS / FAIL
END
```

CPU architectural-state-sensitive sequences are not interrupted by logging calls. If the emulator host terminates, the final emitted `CKPT <test-id> <checkpoint-id>` identifies the last confirmed guest boundary.

## Stable tests

1. `CPU.NZCV.CINC.001`
2. `CPU.NZCV.CSEL.001`
3. `CPU.REG.PRESERVE.001`
4. `IPC.SM.GET_SERVICE.001`
5. `IPC.SVC21.REPEATED.001`

No testcase ID was added during crash-observability stabilization.

## Selection

Default `NCE-DIAG.nro`: registry indices 1-5.

CINC-only executable: `NCE-DIAG-CINC.nro`.

Runtime arguments:

```text
--all
--test=CPU.NZCV.CINC.001
--range=1-3
--persist
--debug-only
--config
--no-config
```

Filesystem config is deliberately opt-in. `nce_diag.cfg` is not read during default startup.

## Persistence

Default mode is debug-only: no filesystem is initialized and no JSON is written.

`--persist` or explicit config mode lazily enables FS/SDMC after the initial guest debug markers. Persistent file output is not the crash-boundary channel; direct SVC debug output is authoritative for host-process termination.

Persistent mode can write:

- `sdmc:/nce_diag_journal.log`
- `sdmc:/nce_diag_result.json`

with local-path fallbacks.

## Build

Requirements: devkitPro, devkitA64, libnx.

Full suite executable:

```sh
make
```

CINC-only executable without source edits:

```sh
make TARGET=NCE-DIAG-CINC BUILD=build-cinc NCE_DIAG_DEFAULT_FIRST=1 NCE_DIAG_DEFAULT_LAST=1
```

General compile-time range preset:

```sh
make TARGET=NCE-DIAG-RANGE BUILD=build-range NCE_DIAG_DEFAULT_FIRST=2 NCE_DIAG_DEFAULT_LAST=4
```

CI verifies that the CINC `main()` does not read `CNTPCT`/`CNTVCT` and that `build/harness.o` has no dependency on `snprintf`, `vsnprintf`, `_svfprintf_r`, `strlen`, `memset`, or `malloc`.

See `docs/CHECKPOINTS.md`, `docs/RUN_SELECTION.md`, `docs/TESTS.md`, and `docs/STARTUP_CRASH_20260912.md`.
