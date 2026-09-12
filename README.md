# NCE-DIAG

Independent internal guest-side conformance and diagnostic workload for Windows ARM64 NCE validation.

NCE-DIAG is an executable diagnostic application, not a source-only or raw-blob project. The primary deliverable is `NCE-DIAG.nro`, which runs the test suite automatically, prints PASS/FAIL, writes a machine-readable result file, and exits without user input.

Eden/Strato production implementation repositories are reference-only from this project.

## 0.1 executable target

Current build candidate contains all first-milestone tests:

| Test ID | Semantic | Implementation | Runtime validation |
| --- | --- | --- | --- |
| `CPU.NZCV.CINC.001` | NZCV preservation through x18-sensitive STP before CINC | embedded raw ARM64 routine | pending |
| `CPU.NZCV.CSEL.001` | conditional select and NZCV preservation | ARM64 routine | pending |
| `CPU.REG.PRESERVE.001` | x19-x28 preservation across direct BL/RET | ARM64 routine | pending |
| `IPC.SM.GET_SERVICE.001` | real SM GetService moved-handle path | libnx CMIF over real SM session | pending |
| `IPC.SVC21.REPEATED.001` | repeated synchronous IPC on one SM session | 16 repeated CMIF GetService requests | pending |

## Automatic execution flow

```text
NCE-DIAG.nro
  -> start
  -> run all five tests
  -> print per-test PASS/FAIL
  -> print summary
  -> write sdmc:/nce_diag_result.json
     (fallback: nce_diag_result.json)
  -> exit automatically
```

No controller input is required.

## Build baseline

Validated GitHub Actions run: `34676758424`

Validated source HEAD for the executable artifact: `b557e6139dae7b92182c4f51df3bd51793912406`

Primary artifact: `NCE-DIAG-0.1-executable`, artifact ID `10292253583`.

`NCE-DIAG.nro` SHA-256:

```text
ed16d5872ca1b3ddab3aebd9388db448fb2b0b054047a1ea0941007f66c026d7
```

CI verifies:

- `NCE-DIAG.nro` exists and is non-empty
- embedded raw CINC symbol exists in the executable ELF
- CSEL and register-preservation assembly symbols exist
- all five stable test IDs are present in the NRO
- result-file path is present
- auxiliary raw CINC microtest still matches the intended instruction sequence

## Build

Requirements: devkitPro, devkitA64, libnx.

```sh
make
```

Primary output:

```text
NCE-DIAG.nro
```

The standalone raw ELF/BIN remains an auxiliary development artifact only. It is not the user-facing completion target.

## Completion state

Executable build/packaging: PASS.

Eden/real-Switch runtime execution: pending.

NCE-DIAG 0.1 is not considered complete until the same NRO boots in Eden, executes all five tests, writes the result file, and exits normally.
