# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.2-crash-observability`

Role: independent internal guest-side NCE diagnostic executable. Eden/Strato production implementation remains read-only from this project.

0.2 scope: crash observability only; no new testcase IDs added.

Implemented:

- direct `svcOutputDebugString` primary logging
- run ID on every diagnostic line
- startup markers
- testcase `BEGIN`, internal stable `CKPT`, immediate `PASS`/`FAIL`, `END`
- CPU raw-sequence checkpoints outside architectural-state-sensitive assembly
- explicit IPC request-build / PRE_SVC / POST_SVC / parse boundaries
- repeated IPC iteration number in checkpoint output
- optional flushed lifecycle journal; internal CKPT remains filesystem-free
- suite, single-test, and 1-based range selection
- GUI console initialization removed
- normal-completion JSON result retained

Build validation at source commit `86d7eef1577a568a92e9c63606b3beb865adc94a` passed executable-shape and auxiliary raw-shape checks in workflow run `34677885223` before this documentation-only commit.

Runtime status:

- previous 0.1 NRO: Eden load/boot and guest execution observed, then host process crash
- 0.2 crash-observable NRO: build validated; Eden runtime checkpoint capture pending

Do not add new tests until the 0.2 NRO is run in Eden and the last guest-side checkpoint is captured.
