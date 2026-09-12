# 2026-09-12 pre-main startup crash

## Observation

The crash-observable 0.2 NRO loaded and Windows ARM64 NCE execution began, but no normal `[NCE-DIAG][RUN=...]` marker was emitted before the Eden host process terminated.

## Guest PC symbolization

Using the matching NCE-DIAG ELF and image base derived from the runtime PC range:

| Guest offset | Symbol/source |
| --- | --- |
| `0x7000` | `serviceClose` inside applet cleanup path |
| `0x9308` | `cmifMakeRequest` |
| `0x9334` | `hipcParseResponse` |
| `0x93AC` | `_fsCmdGetSession`, libnx `fs.c` |
| `0x93E0` | `serviceCreateDomainSubservice` |

The final observed PC was `+0x93AC`, before `main()`.

## Root startup dependency

libnx 4.12 default weak `__appInit` initializes SM, system settings, applet, HID, time, FS, and SDMC before `main()`.

`fsdevMountSdmc()` calls `fsOpenSdCardFileSystem()`, which uses `_fsCmdGetSession` to obtain the SD-card filesystem object.

Therefore NCE-DIAG could crash in default library startup before its own test harness had any chance to emit a marker.

## 0.2.1 response

NCE-DIAG now supplies strong `__appInit` / `__appExit` overrides. Default pre-main service initialization is SM-only. FS/SDMC is moved to an explicit late optional path used only by config/persistence.

Classification of the original run:

```text
HOST_PROCESS_CRASH_BEFORE_MAIN_DURING_LIBNX_FS_STARTUP
```

No testcase should be marked FAIL from that run.
