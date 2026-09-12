# Test registry

Stable 1-based registry order:

| Index | Test ID | 0.2 checkpoint focus |
| ---: | --- | --- |
| 1 | CPU.NZCV.CINC.001 | `10_PRE_SEQUENCE -> 20_POST_SEQUENCE -> 30_RESULT_CAPTURED` |
| 2 | CPU.NZCV.CSEL.001 | `10_PRE_SEQUENCE -> 20_POST_SEQUENCE -> 30_RESULT_CAPTURED` |
| 3 | CPU.REG.PRESERVE.001 | `10_PRE_SEQUENCE -> 20_POST_SEQUENCE -> 30_RESULT_CAPTURED` |
| 4 | IPC.SM.GET_SERVICE.001 | `20_REQUEST_BUILT -> 30_PRE_SVC -> 40_POST_SVC -> 50_RESULT_PARSED` |
| 5 | IPC.SVC21.REPEATED.001 | same IPC boundary plus `iter=NN`, repeated 16 times |

Each testcase also receives harness-level `00_ENTER` and `90_RESULT_RETURNED` checkpoints plus immediate `PASS`/`FAIL` and `END` lifecycle markers.

## CPU.NZCV.CINC.001

Input remains `w16 = 0`; expected result remains `2`; expected NZCV before and after the x18-sensitive store pair remains `0x60000000`.

The raw architectural sequence is not interrupted by logging. `10_PRE_SEQUENCE` is emitted before entering the assembly routine and `20_POST_SEQUENCE` only after the routine returns.

## IPC SVC boundary

The two existing IPC tests now explicitly separate CMIF request creation from `svcSendSyncRequest` and response parsing. This makes SVC 0x21 crashes distinguishable from request-build and response-parse crashes without changing the stable testcase IDs.
