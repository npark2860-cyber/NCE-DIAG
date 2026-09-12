# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

NCE-DIAG is not a public homebrew product. Its purpose is to reduce large-game failures to small deterministic tests that exercise one CPU, SVC, IPC, exception, or memory semantic at a time and report expected/actual state.

The project is intentionally separate from Eden/Strato production implementation code. Eden/Strato may be used as reference and comparison targets, but failures found here are not fixed in those repositories from this project.

NCE-DIAG is not limited to libnx abstractions. Public devkitA64/libnx/switchbrew material remains useful for bootstrap paths, while authorized internal references such as user-provided firmware/title dumps, NSO/NPDM/ExeFS, ABI/API material, binaries, disassembly, logs, known-good runtime captures, and real-hardware results may also be used when needed.

## Test status

| Test ID | Semantic | Implementation | Build | Runtime |
| --- | --- | --- | --- | --- |
| `CPU.NZCV.CINC.001` | NZCV preservation across x18-sensitive store-pair sequence | raw + NRO | PASS | pending |
| `CPU.NZCV.CSEL.001` | conditional select | planned | - | - |
| `CPU.X18.TRANSITION.001` | x18-sensitive transition | planned | - | - |
| `IPC.SM.GET_SERVICE.001` | GetService handle path | planned | - | - |
| `IPC.HIPC.MOVE_HANDLE.001` | moved handle parsing | planned | - | - |
| `IPC.DOMAIN.CONVERT.001` | session to domain | planned | - | - |
| `EXC.RESUME.001` | exception resume | planned | - | - |

## CPU.NZCV.CINC.001

The semantic core is shared by the raw microtest and NRO runner:

```asm
cmp     w16, #0
mrs     x10, nzcv
stp     w17, w18, [base, #0x0c]
mrs     x11, nzcv
mov     w2, #2
cinc    w2, w2, ne
```

For `w16 = 0`:

- expected result: `2`
- expected NZCV before `stp`: `0x60000000`
- expected NZCV after `stp`: `0x60000000`

### Raw layer

`microtests/cpu_nzcv_cinc_001.s` builds to standalone ELF/BIN artifacts without libc/libnx/C runtime dependency. The runner supplies a writable result buffer in `x0`. See `docs/RAW_MICROTEST_ABI.md`.

### NRO bootstrap layer

The devkitA64/libnx NRO provides a convenient screen runner for the same semantic. This is a bootstrap choice, not a permanent API restriction.

```sh
make
make -C microtests
```

Current build validation:

- workflow run: `34676469257`
- validated SHA: `564fc12b7389f672cb50f0137c01f4183b061784`
- NRO artifact: `10292192995`
- raw artifact: `10292208003`

Runtime differential validation is the next step. No second semantic should be added before this first raw/NRO pair is executed in a known-good reference and Windows ARM64 Eden NCE.
