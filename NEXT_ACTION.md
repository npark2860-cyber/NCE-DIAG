# NEXT_ACTION

Do not add new testcase IDs.

Validated x19-only witness build: `7c32133fb45c075197677eb71af786bee84837d5`.
Validated CI run: `34703870644` PASS.
Executable artifact ID: `10301306905`.

Authoritative runtime result:

```text
CPU.NZCV.CINC.001 / 00_ENTER SVC 0x27 returned
CPU.NZCV.CINC.001 / 10_PRE_SEQUENCE entered SVC 0x27
no matching RETURN
raw CINC sequence was not entered
```

Observer-side ambiguity is resolved:

- current witness changes x19 only
- x20 is not touched by the witness emitter
- the prior checkpoint-adjacent x20-induced unmapped signature is gone
- therefore this boundary should now be treated as a production Eden/NCE failure boundary, not a diagnostic-harness artifact

Next action:

1. Do not modify NCE-DIAG further for this failure.
2. Hand the boundary to the Windows ARM64 NCE production investigation.
3. Production-side target is the second project-owned `svcOutputDebugString` / SVC 0x27 re-entry: `10_PRE_SEQUENCE`.
4. Do not classify the host exit as guest `TEST_FAIL`.
5. Do not investigate the raw CINC instruction sequence yet; it was not entered.
6. After a production Eden/NCE change is available, rerun the same `NCE-DIAG-CINC.nro` unchanged.
7. Only if `10_PRE_SEQUENCE` returns should classification continue to the next witness (`20_POST_SEQUENCE`) and eventually the full five-test suite.

Known CINC x19 witnesses:

- `00_ENTER` -> `4E434557672A5071`
- `10_PRE_SEQUENCE` -> `4E434557546DD9AD`
- `20_POST_SEQUENCE` -> `4E4345578493EDB7`
- `30_RESULT_CAPTURED` -> `4E434557EA0D5FDC`
- `90_RESULT_RETURNED` -> `4E434557E41DEE7F`
