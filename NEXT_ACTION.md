# NEXT_ACTION

Do not add new testcase IDs.

1. Run the latest `NCE-DIAG-CINC.nro` built after removal of pre-marker `CNTPCT_EL0`.
2. Keep config/persistence disabled; this run must remain debug-only and filesystem-free.
3. Capture the Eden log from boot until exit/crash.
4. Compare the final RunThread sequence with the previous constructor-probe run.
5. Primary success criterion: execution progresses past the old constructor boundary and produces additional SVC 0x27 activity attributable to `main()` / harness logging, then reaches `CPU.NZCV.CINC.001`.
6. If it still terminates before any new main/harness SVC boundary, symbolize the final guest PC against the matching retained ELF before changing another variable.
7. Only after the CINC-only runtime boundary moves may the full suite be retried.
