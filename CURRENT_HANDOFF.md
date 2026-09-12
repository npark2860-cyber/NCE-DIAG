# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent internal guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Runtime evidence

The original 0.2 executable terminated before `main()` in libnx default FS startup. Matching ELF symbolization placed the final guest PC in `_fsCmdGetSession`. 0.2.1 therefore replaced libnx default `__appInit` with an SM-only startup path.

The next two Eden runs (full suite and CINC-only) showed the same normalized SVC progression and no FS startup path, proving the FS bypass was active.

A constructor-only probe was then added. The resulting Eden log continued beyond the previous boundary with additional SVC 0x27 RunThread returns. The current Eden RunThread diagnostic can observe one SVC boundary more than once, so these return records are not counted as one-to-one textual checkpoint lines; they are treated only as proof of additional guest progress.

Matching `NCE-DIAG-CINC.elf` disassembly exposed a stronger pre-marker boundary: `main()` executed `armGetSystemTick()` before its first normal diagnostic marker. libnx implements that helper as `MRS CNTPCT_EL0`. Thus the first architectural system-register read could terminate Windows NCE before `START version=...` was emitted.

## Current implementation

- strong project-owned `__appInit` / `__appExit`
- startup initializes SM only
- direct SVC 0x27 checkpoint channel
- no default applet/HID/time/FS/SDMC/console path before main
- FS remains lazy and opt-in
- full-suite and CINC-only executable NROs
- stable testcase IDs unchanged
- constructor boundary probe retained
- pre-marker run-id generation no longer reads `CNTPCT_EL0`; it uses address/integer entropy only
- CI disassembles CINC `main()` and rejects `CNTPCT`/`CNTVCT` reads before accepting the build
- ELF/map artifacts retained for guest-PC symbolization

Build validation: workflow run `34679720750` PASS at source commit `39a7fcd93b2c74ad58eda17d4338bcc1ff820dc6`. Manual artifact disassembly also confirmed no `cntpct`/`cntvct` instruction in CINC `main()`.

Runtime status: the no-CNTPCT build is the next required Eden run. Do not add new testcase IDs before this result is captured.
