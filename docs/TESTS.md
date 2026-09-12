# Test registry

| Test ID | Input / action | Expected | Runtime status |
| --- | --- | --- | --- |
| `CPU.NZCV.CINC.001` | `w16=0`, CMP -> x18-sensitive STP -> CINC NE | result `2`, NZCV `0x60000000` preserved | pending |
| `CPU.NZCV.CSEL.001` | compare `5` and `7`, CSEL LT | result `5`, NZCV `0x80000000` preserved | pending |
| `CPU.REG.PRESERVE.001` | seed x19-x28, direct BL/RET, compare | mismatch mask `0` | pending |
| `IPC.SM.GET_SERVICE.001` | CMIF `sm:GetServiceOriginal("fsp-srv")` | valid moved handle, close succeeds | pending |
| `IPC.SVC21.REPEATED.001` | 16 repeated GetService requests on the same SM session | 16/16 complete | pending |

All five tests are included in the executable NRO and are executed automatically in the order above.

## Output contract

Each test returns:

- stable ID
- PASS/FAIL/SKIP
- expected
- actual
- Result code
- NZCV before/after when relevant
- diagnostic detail string

The executable writes a JSON result object containing summary counts and all test records.
