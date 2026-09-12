# NEXT_ACTION

Do not add new testcase IDs.

1. Run `NCE-DIAG-CINC.nro` first in the same Windows ARM64 Eden build.
2. Capture the Eden log from boot until exit/crash.
3. Expected earliest markers are:
   - `[NCE-DIAG][BOOT] 00_APP_INIT_ENTER`
   - `[NCE-DIAG][BOOT] 10_PRE_SM_INIT`
   - `[NCE-DIAG][BOOT] 20_POST_SM_INIT`
   - `[NCE-DIAG][BOOT] 30_APP_INIT_DONE`
   - `[NCE-DIAG][RUN=...] START version=0.2.1`
4. If CINC-only reaches PASS/END, run `NCE-DIAG.nro` full suite.
5. If Eden terminates before a normal marker, symbolize the last NCE guest PC against the retained `NCE-DIAG-0.2.1-debug-symbols` artifact.
6. Do not enable `--config` or `--persist` during the first retry; both intentionally enter the optional FS path.
