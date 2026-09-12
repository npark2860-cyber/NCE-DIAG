# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

## 0.2.1 crash-observability runtime

Primary artifact: `NCE-DIAG.nro`.

A second executable, `NCE-DIAG-CINC.nro`, runs registry index 1 only (`CPU.NZCV.CINC.001`) and is intended for the first isolated Eden retry.

The primary diagnostic channel is direct `svcOutputDebugString` (SVC 0x27). GUI rendering is not required.

### Minimal startup

The project overrides libnx's weak `__appInit` / `__appExit`.

Default startup is now:

```text
libnx core runtime
-> [BOOT] marker
-> SM initialize only
-> [BOOT] marker
-> main
-> test harness
```

Default startup does not initialize applet, HID, time, FS, SDMC, or a console framebuffer.

This change was made after the previous 0.2 runtime log was symbolized to `_fsCmdGetSession` before `main()`, consistent with libnx default SD-card filesystem startup.

### Test lifecycle

Every testcase follows:

```text
BEGIN
CKPT before dangerous operation
CKPT after dangerous operation
PASS / FAIL
END
```

CPU architectural-state-sensitive sequences are not interrupted by logging calls.

## Stable tests

1. `CPU.NZCV.CINC.001`
2. `CPU.NZCV.CSEL.001`
3. `CPU.REG.PRESERVE.001`
4. `IPC.SM.GET_SERVICE.001`
5. `IPC.SVC21.REPEATED.001`

No new testcase ID was added for 0.2.1.

## Selection

Default `NCE-DIAG.nro`: registry indices 1-5.

CINC-only executable: `NCE-DIAG-CINC.nro`.

Runtime arguments remain available:

```text
--all
--test=CPU.NZCV.CINC.001
--range=1-3
--persist
--debug-only
--config
```

Filesystem config is deliberately opt-in. `nce_diag.cfg` is not read during default startup.

## Persistence

Default mode is debug-only: no filesystem is initialized and no JSON is written.

`--persist` or explicit config mode lazily enables FS/SDMC after the initial guest debug markers. FS startup itself has PRE/POST markers so an emulator crash there is observable.

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

See `docs/CHECKPOINTS.md`, `docs/RUN_SELECTION.md`, `docs/TESTS.md`, and `docs/STARTUP_CRASH_20260912.md`.
