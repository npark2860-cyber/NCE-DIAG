# Test registry

| Test ID | Semantic | Raw | NRO | Build | Runtime |
| --- | --- | --- | --- | --- | --- |
| CPU.NZCV.CINC.001 | Preserve NZCV across the x18-sensitive store-pair sequence before `cinc` | implemented | implemented | PASS | pending |
| CPU.NZCV.CSEL.001 | Conditional select | planned | planned | - | - |
| CPU.REG.PRESERVE.001 | Register preservation | planned | planned | - | - |
| IPC.SM.GET_SERVICE.001 | Service handle return | planned | planned | - | - |
| IPC.SVC21.REPEATED.001 | Repeated session IPC | planned | planned | - | - |

## CPU.NZCV.CINC.001

Input: `w16 = 0`.

Expected result: `2`.

Expected NZCV immediately after `cmp w16, #0`: `0x60000000` (`Z=1`, `C=1`).

The shared semantic core snapshots NZCV before and after `stp w17, w18, [base, #0x0c]`. Both snapshots must remain `0x60000000`, and `cinc ..., ne` must not increment the value.

Raw result ABI is documented in `RAW_MICROTEST_ABI.md`.

Build validation:

- workflow run `34676469257`: PASS
- validated SHA `564fc12b7389f672cb50f0137c01f4183b061784`
- NRO artifact `10292192995`
- raw artifact `10292208003`

Runtime reference comparison remains pending.
