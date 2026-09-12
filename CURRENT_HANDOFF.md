# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent internal guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Runtime evidence

The original 0.2 executable terminated before `main()` in libnx default FS startup. A project-owned minimal `__appInit` now initializes SM only and leaves FS/SDMC opt-in.

Subsequent CINC-only runs proved guest execution progressed through startup and into the diagnostic output path. The latest probe run reached repeated SVC 0x27 boundaries but showed that normal diagnostic formatting itself entered newlib `snprintf/vsnprintf -> _svfprintf_r` dependency code before useful testcase text could be trusted as the crash boundary.

## Current implementation

- strong project-owned `__appInit` / `__appExit`
- startup initializes SM only
- direct SVC 0x27 debug channel
- no default applet/HID/time/FS/SDMC/console path before main
- FS remains lazy and opt-in
- full-suite and CINC-only executable NROs
- stable testcase IDs unchanged
- pre-marker run-id generation uses ordinary address/integer entropy, not `CNTPCT_EL0`
- critical `nce_diag_logf`, `nce_diag_checkpoint`, and `nce_diag_checkpointf` formatting is project-owned and does not call `snprintf`, `vsnprintf`, or `_svfprintf_r`
- temporary main/constructor/svfprintf dependency probe wrappers have been removed from the stable diagnostic path
- CI checks the CINC `main()` for `CNTPCT`/`CNTVCT` and checks `build/harness.o` for forbidden formatted-libc dependencies
- ELF/map artifacts are retained for guest-PC symbolization

## Runtime status

The next required run is the CINC-only executable produced from the libc-independent checkpoint commit. Success is defined as observing named NCE-DIAG lifecycle/checkpoint text through `CPU.NZCV.CINC.001`; if Eden terminates, the last emitted `CKPT <test-id> <checkpoint-id>` is the authoritative diagnostic boundary.
