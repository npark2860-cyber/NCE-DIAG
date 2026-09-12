# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

Current version: `0.2.1`.

## Latest confirmed runtime

The x19-only witness CINC run establishes the current authoritative boundary:

```text
CPU.NZCV.CINC.001 / 00_ENTER entered SVC 0x27 and returned
CPU.NZCV.CINC.001 / 10_PRE_SEQUENCE entered SVC 0x27
no matching RunThread RETURN follows
raw CINC sequence was not entered
```

Observed x19 witness values:

- `00_ENTER`: `4E434557672A5071`
- `10_PRE_SEQUENCE`: `4E434557546DD9AD`

The final Eden record is the SVC 0x27 PRE for `10_PRE_SEQUENCE`.

This direct witness supersedes the earlier ELF-only estimate that the run had reached `20_POST_SEQUENCE`.

## Observer-side ambiguity resolved

An earlier two-register witness temporarily changed x20 and produced observer-induced unmapped-memory diagnostics. The current implementation uses an x19-only emitter.

Source inspection confirms `nce_diag_emit_debug_witness_x19`:

- saves/restores x19
- writes only x19 with the checkpoint witness
- never reads or writes x20
- preserves x30 around `svcOutputDebugString`

In the latest run, the prior checkpoint-adjacent x20 witness/unmapped signature is gone. The only early unmapped diagnostic is the pre-existing initial stack probe at RunThread `n=0`, not a checkpoint witness effect.

Therefore the repeated failure at `10_PRE_SEQUENCE` is no longer attributable to NCE-DIAG modifying x20.

## Current validated build

Implementation HEAD: `7c32133fb45c075197677eb71af786bee84837d5`

GitHub Actions run `34703870644`: PASS.

Validation includes:

- default and CINC-only NRO build
- libc-independent crash checkpoint path
- x19-only witness source linked into the harness path
- existing witness decoder compatibility
- auxiliary raw instruction-shape validation
- executable/debug-symbol/raw-microtest artifact upload

Executable artifact: `NCE-DIAG-0.2.1-executables`, artifact ID `10301306905`, SHA-256 digest `8885c48f468d1534145bf46a9810704a45147573f9c1a29de0cb1dfa8b23745a`.

## Diagnostic status

The crash-observability goal for the current boundary is satisfied. Do not add new testcase IDs and do not add further witness mechanisms for this failure.

The current issue should be handed to the production Windows ARM64 NCE investigation as:

```text
NCE-DIAG-CINC
00_ENTER SVC 0x27 returns
10_PRE_SEQUENCE SVC 0x27 enters
host terminates before matching return
raw CINC sequence never executes
x19-only observer confirmed; x20 untouched by witness
```

Resume NCE-DIAG runtime testing only after a production Eden/NCE change is available to validate this boundary.