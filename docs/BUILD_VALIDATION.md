# Build validation

NCE-DIAG 0.2 CI must prove that the primary executable artifact exists and that crash-observability machinery is linked into that executable.

Required build checks:

- `NCE-DIAG.nro` exists
- `NCE-DIAG.elf` exists
- embedded raw CINC routine is linked
- CSEL and register-preservation probes are linked
- `nce_diag_checkpoint` is linked
- `svcOutputDebugString` is linked
- `consoleInit` is not linked by the diagnostic harness
- stable test IDs and PRE/POST checkpoint strings are present
- run-selection config/result/journal path strings are present
- auxiliary raw CINC instruction-shape verifier still passes

A successful CI build does not prove Eden runtime behavior. Eden boot and last-checkpoint capture remain runtime validation steps.
