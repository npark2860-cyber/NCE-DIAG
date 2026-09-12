# NEXT_ACTION

Do not add the second CPU test yet.

Next action is only to validate the current minimal stage:

1. build `NCE-DIAG.nro` with devkitA64/libnx;
2. if the build passes, run the same NRO in one known-good reference environment;
3. run the same NRO in Windows ARM64 Eden NCE;
4. record the observed `CPU.NZCV.CINC.001` result and NZCV values;
5. only after this first probe is validated, add `CPU.NZCV.CSEL.001`.
