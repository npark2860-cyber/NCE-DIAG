# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.1-minimal`

Current HEAD before this documentation commit: `b557e6139dae7b92182c4f51df3bd51793912406`.

Project role: internal executable guest-side conformance/diagnostic bench for Windows ARM64 NCE. Eden/Strato production code is not modified here.

## Current executable state

`NCE-DIAG.nro` is now the primary target and contains all first-milestone tests:

- `CPU.NZCV.CINC.001`
- `CPU.NZCV.CSEL.001`
- `CPU.REG.PRESERVE.001`
- `IPC.SM.GET_SERVICE.001`
- `IPC.SVC21.REPEATED.001`

Runtime behavior implemented:

1. start automatically;
2. execute all five tests;
3. print per-test PASS/FAIL and summary;
4. print detailed state for failures;
5. write JSON to `sdmc:/nce_diag_result.json`, with relative-path fallback;
6. exit automatically after output flush.

The CINC raw ARM64 microtest is embedded directly in the NRO and called by the harness. The standalone raw ELF/BIN remains auxiliary only.

## Validated build baseline

GitHub Actions run: `34676758424` — PASS.

Executable artifact ID: `10292253583` (`NCE-DIAG-0.1-executable`).

Artifact ZIP digest: `sha256:ce3ca0999b176bb97f05e1d9162686b6147bf5023b8de495fb90ecab1c51051d`.

Extracted `NCE-DIAG.nro` SHA-256:
`ed16d5872ca1b3ddab3aebd9388db448fb2b0b054047a1ea0941007f66c026d7`.

CI verified executable build, embedded test symbols, all five IDs, JSON output path, and auxiliary raw instruction shape.

## Not yet claimed

- Eden boot PASS
- real Switch boot PASS
- actual five-test runtime results
- JSON file creation observed on target
- normal target exit observed

Therefore first milestone is build-complete but runtime-open. Do not declare NCE-DIAG 0.1 complete until target execution evidence is captured.
