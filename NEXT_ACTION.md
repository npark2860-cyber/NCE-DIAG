# NEXT_ACTION

Do not add the second CPU semantic yet.

Current order:

1. validate the NRO bootstrap and raw `CPU.NZCV.CINC.001` artifacts in CI;
2. run the same semantic in one known-good reference environment;
3. run it in Windows ARM64 Eden NCE;
4. compare `actual_result`, `nzcv_before`, and `nzcv_after`;
5. record the differential result;
6. only then add `CPU.NZCV.CSEL.001`.

The raw microtest is the precision reference. The NRO path is the convenient bootstrap runner.
