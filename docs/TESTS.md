# Test registry

| Test ID | Semantic | Implementation | Runtime validation |
| --- | --- | --- | --- |
| CPU.NZCV.CINC.001 | Preserve NZCV across the x18-sensitive store-pair sequence before `cinc` | implemented | pending |
| CPU.NZCV.CSEL.001 | Conditional select | planned | pending |
| CPU.REG.PRESERVE.001 | Register preservation | planned | pending |
| IPC.SM.GET_SERVICE.001 | Service handle return | planned | pending |
| IPC.SVC21.REPEATED.001 | Repeated session IPC | planned | pending |

## CPU.NZCV.CINC.001

Input: `w16 = 0`.

Expected result: `2`.

Expected NZCV immediately after `cmp w16, #0`: `0x60000000` (`Z=1`, `C=1`).

The test snapshots NZCV before and after `stp w17, w18, [x0, #0x0c]`. Both snapshots must remain `0x60000000`, and `cinc ..., ne` must not increment the value.
