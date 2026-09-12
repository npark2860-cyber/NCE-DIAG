# NEXT_ACTION

Do not add new testcase IDs.

Validated x19-only witness build: `7c32133fb45c075197677eb71af786bee84837d5`.
Validated CI run: `34703870644` PASS.
Executable artifact ID: `10301306905`.

Latest runtime result:

```text
CPU.NZCV.CINC.001 / 00_ENTER returned
CPU.NZCV.CINC.001 / 10_PRE_SEQUENCE entered SVC 0x27
no matching RETURN
raw CINC sequence was not entered
```

The previous safe witness still modified x20. Eden probed state relative to that temporary x20 value and produced observer-induced unmapped reads. The new harness path leaves x20 completely unchanged and carries the combined checkpoint witness only in x19.

Next action:

1. Run `NCE-DIAG-CINC.nro` from artifact `10301306905` with config/persistence disabled.
2. Capture the Eden log through normal exit or host termination.
3. Use the final SVC 0x27 PRE record with `x19=4E434557xxxxxxxx` as the checkpoint identity.
4. Confirm that the prior x20-related unmapped-read signature is gone.
5. If `10_PRE_SEQUENCE` returns, continue classification using the next x19 witness. If it does not return, treat the SVC 0x27 boundary itself as the current host-failure boundary.
6. Only after CINC-only is runtime-stable run the full five-test suite unchanged.

Known CINC x19 witnesses:

- `00_ENTER` -> `4E434557672A5071`
- `10_PRE_SEQUENCE` -> `4E434557546DD9AD`
- `20_POST_SEQUENCE` -> `4E4345578493EDB7`
- `30_RESULT_CAPTURED` -> `4E434557EA0D5FDC`
- `90_RESULT_RETURNED` -> `4E434557E41DEE7F`
