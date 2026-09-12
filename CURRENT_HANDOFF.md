# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Latest confirmed runtime

Latest CINC-only Eden log proves:

- `CPU.NZCV.CINC.001 / 00_ENTER` reached SVC 0x27 and returned.
- `CPU.NZCV.CINC.001 / 10_PRE_SEQUENCE` reached SVC 0x27.
- No matching RunThread RETURN follows that final PRE record.
- Therefore the raw CINC sequence was not entered in that run.

The combined x19 witnesses were:

- `00_ENTER`: `4E434557672A5071`
- `10_PRE_SEQUENCE`: `4E434557546DD9AD`

This direct witness supersedes the earlier ELF-only estimate that the run had reached `20_POST_SEQUENCE`.

## Witness correction

The previous safe witness still changed x20 temporarily. Eden's Windows NCE diagnostics inspect state relative to x20 during SVC re-entry, so that observer change produced extra unmapped-memory diagnostics.

The current harness now uses an x19-only witness emitter:

- checkpoint identity is carried only in x19
- x20 is not modified by the witness emitter
- original x19 is restored if SVC 0x27 returns
- stable testcase IDs are unchanged

## Current validated build

Implementation HEAD: `7c32133fb45c075197677eb71af786bee84837d5`

GitHub Actions run `34703870644`: PASS.

The run passed:

- default and CINC-only NRO build
- existing crash-observability validation
- auxiliary raw instruction-shape validation
- executable/debug-symbol/raw-microtest artifact upload

Executable artifact: `NCE-DIAG-0.2.1-executables`, artifact ID `10301306905`, SHA-256 digest `8885c48f468d1534145bf46a9810704a45147573f9c1a29de0cb1dfa8b23745a`.

## Next runtime

Run `NCE-DIAG-CINC.nro` from artifact `10301306905` with config/persistence disabled. The next log should preserve the x19 checkpoint witness while leaving x20 untouched.
