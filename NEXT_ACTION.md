# NEXT_ACTION

Do not add new testcase IDs.

1. Download the latest `NCE-DIAG-0.2-crash-observable` artifact.
2. First run only `CPU.NZCV.CINC.001` using `--test=CPU.NZCV.CINC.001` or `nce_diag.cfg`.
3. Capture Eden log lines beginning with `[NCE-DIAG]`.
4. If Eden exits, classify the last CKPT and the next expected CKPT as the host-crash boundary.
5. If the single CPU test completes, run the full suite.
6. Only after runtime evidence is recorded may the 0.2 harness milestone be marked PASS or the next semantic be added.
