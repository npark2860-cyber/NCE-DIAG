# NCE-DIAG

Independent guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

The workload is intentionally separate from Eden/Strato implementation code and uses public devkitA64/libnx tooling. The first implementation is deliberately small: one harness and one deterministic CPU regression probe.

## Current test

`CPU.NZCV.CINC.001`

The probe fixes the guest instruction shape in AArch64 assembly:

```asm
cmp     w16, #0
mrs     x10, nzcv
stp     w17, w18, [x0, #0x0c]
mrs     x11, nzcv
mov     w2, #2
cinc    w2, w2, ne
```

For `w16 = 0` the expected state is:

- result: `2`
- NZCV before `stp`: `0x60000000`
- NZCV after `stp`: `0x60000000`

The probe saves and restores the incoming architectural `x18` value before returning to the C harness.

## Build

Requirements:

- devkitPro
- devkitA64
- libnx
- `DEVKITPRO` environment variable configured

Build:

```sh
make
```

Expected artifact:

```text
NCE-DIAG.nro
```

Clean:

```sh
make clean
```

## Run

Place `NCE-DIAG.nro` under the normal Switch homebrew path and launch it through hbmenu or the equivalent homebrew loader in the target environment.

The current stage prints the test result and detailed NZCV values on failure. Machine-readable file output is intentionally deferred until this first probe is build- and runtime-validated.
