# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.1-minimal`

Project role: private/internal guest-side conformance and diagnostic bench for Windows ARM64 NCE. It is not constrained to public homebrew APIs. Authorized internal references may be used when needed, while Eden/Strato production code remains read-only from this project.

Current stage: minimal harness plus `CPU.NZCV.CINC.001` only.

Implemented:

- devkitA64/libnx NRO bootstrap Makefile structure
- common `TestResult` harness shape
- fixed AArch64 assembly probe for the first NZCV/CINC regression
- human-readable PASS/FAIL summary and detailed failure state
- GitHub Actions NRO build validation path

Static validation performed before commit:

- AArch64 assembly accepted by Clang 17 targeting `aarch64-none-elf`
- disassembly confirmed the intended `cmp / mrs / stp w17,w18 / mrs / mov / cinc` sequence
- C harness passed `-Wall -Wextra -Werror` syntax checking against a minimal libnx API stub

Not yet claimed:

- devkitA64/libnx full NRO build PASS
- execution PASS on a known-good reference or Windows ARM64 Eden NCE
- JSON/text machine-readable result output
- any test beyond `CPU.NZCV.CINC.001`

Do not add the next semantic until the current probe is built and runtime-validated.
