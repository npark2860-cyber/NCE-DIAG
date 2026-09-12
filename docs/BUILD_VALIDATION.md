# Build validation

NCE-DIAG 0.2.1 CI validates both the full-suite executable and a CINC-only preset.

Required checks:

- `NCE-DIAG.nro` and `NCE-DIAG.elf` exist
- `NCE-DIAG-CINC.nro` and `NCE-DIAG-CINC.elf` exist
- project override symbols `__appInit` and `__appExit` are strong `T` symbols
- `nce_diag_filesystem_enable` is linked for optional late FS startup
- embedded raw CINC routine is linked
- `nce_diag_checkpoint` and `svcOutputDebugString` are linked
- `consoleInit` is not linked by the diagnostic harness
- pre-main BOOT marker strings are present
- PRE/POST testcase marker strings are present
- auxiliary raw CINC instruction-shape verifier passes
- ELF and linker maps are retained for guest-PC symbolization

Validated source commit: `2f52e25135860e6fe25b9d027128dd2f4696810e`.

Validated workflow run: `34678637086` PASS.

Successful CI does not prove Eden runtime behavior. The next required evidence is a CINC-only Eden run.
