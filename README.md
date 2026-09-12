# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

NCE-DIAG is not a public homebrew product. Its purpose is to reduce large-game failures to small deterministic tests that exercise one CPU, SVC, IPC, exception, or memory semantic at a time and report expected/actual state.

The project is intentionally separate from Eden/Strato production implementation code. Eden/Strato may be used as reference and comparison targets, but failures found here are not fixed in those repositories from this project.

NCE-DIAG is not limited to libnx abstractions. Public devkitA64/libnx/switchbrew material remains useful for the first executable path, while authorized internal references such as user-provided firmware/title dumps, NSO/NPDM/ExeFS, ABI/API material, binaries, disassembly, logs, known-good runtime captures, and real-hardware results may also be used when needed.

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

## Current bootstrap build path

The first executable path uses devkitA64/libnx because it is small and reproducible; this is a bootstrap choice, not a permanent API restriction.

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

Machine-readable output, IPC/SVC probes, exception probes, memory probes, and any lower-level packaging/runtime path are added incrementally after the current first probe is build- and runtime-validated.
