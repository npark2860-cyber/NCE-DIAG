# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.1-minimal`

Project role: private/internal guest-side conformance and diagnostic bench for Windows ARM64 NCE. It is not constrained to public homebrew APIs. Authorized internal references may be used when needed, while Eden/Strato production code remains read-only from this project.

Current stage: minimal harness plus `CPU.NZCV.CINC.001`, now split into two layers:

- NRO/bootstrap harness path
- standalone raw ARM64 microtest artifact

Implemented:

- devkitA64/libnx NRO bootstrap Makefile structure
- common `TestResult` harness shape
- shared AArch64 semantic macro for the exact `cmp / mrs / stp w17,w18,+0x0c / mrs / mov / cinc` core
- raw result ABI (`NceDiagCpuNzcvCincRawResult`)
- standalone raw ELF/BIN microtest build
- human-readable PASS/FAIL summary and detailed failure state
- GitHub Actions build validation for both NRO and raw microtest
- compiled raw instruction-order verification

Validated before raw split:

- NRO CI run `34676110063`: PASS
- artifact `10292806427`: `NCE-DIAG-nro`

Pending after raw split:

- CI validation of the raw/NRO pair
- runtime execution on a known-good reference
- runtime execution on Windows ARM64 Eden NCE
- JSON/result-file layer
- any test beyond `CPU.NZCV.CINC.001`

Do not add the next semantic until the current raw/NRO pair is built and runtime-validated.
