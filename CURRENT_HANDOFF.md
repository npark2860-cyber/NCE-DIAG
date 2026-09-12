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

## Validated build

Implementation commit: `446d0becca38fa6c8f64e83e7cf7bc1541a8be83` (`diag: make crash checkpoints libc-independent`).

GitHub Actions run `34698719211`: PASS.

The run passed:

- default and CINC-only NRO build
- libc-independent crash checkpoint validation
- auxiliary raw instruction-shape validation
- executable/debug-symbol/raw-microtest artifact upload

Executable artifact: `NCE-DIAG-0.2.1-executables`, artifact ID `10299149605`, SHA-256 digest `d052bf3e67f850ccd495ede956753b80736591aac28773d95503c998413f9be4`.

## Runtime status

The next required action is runtime validation of `NCE-DIAG-CINC.nro` from the validated executable artifact with config/persistence disabled.

Success is defined as observing named NCE-DIAG lifecycle/checkpoint text through `CPU.NZCV.CINC.001`. If Eden terminates, the last emitted `CKPT <test-id> <checkpoint-id>` is the authoritative diagnostic boundary.
