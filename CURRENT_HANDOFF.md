# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent internal guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Runtime evidence that changed the design

The previous 0.2 executable loaded in Eden and entered Windows ARM64 NCE, but no `[NCE-DIAG]` marker reached the Eden log before the host process terminated.

The final observed guest PCs were symbolized against the matching NCE-DIAG ELF:

- `+0x9308`: `cmifMakeRequest`
- `+0x9334`: `hipcParseResponse`
- `+0x93AC`: `_fsCmdGetSession`
- earlier `+0x93E0`: `serviceCreateDomainSubservice`

This is before `main()`. libnx 4.12 default `__appInit` initializes FS and mounts SDMC before main, so the diagnostic harness could not emit its first normal marker.

## 0.2.1 implementation

- strong project-owned `__appInit` / `__appExit`
- direct `[NCE-DIAG][BOOT]` markers from `__appInit`
- startup initializes SM only
- no default applet/HID/time/FS/SDMC/console path before main
- FS is lazy and opt-in for config/persistence only
- FS lazy-init has `PRE_FS_INIT`, `POST_FS_INIT`, `PRE_SD_MOUNT`, `POST_SD_MOUNT` markers
- default debug-only run performs no file I/O
- full-suite NRO plus CINC-only NRO from the same source
- stable testcase IDs unchanged
- ELF/map retained in CI for future guest-PC symbolization

Build validation: workflow run `34678637086` PASS at source commit `2f52e25135860e6fe25b9d027128dd2f4696810e`.

Runtime status: 0.2.1 Eden retry pending. Do not add new testcase IDs before this retry.
