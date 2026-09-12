# Raw microtest ABI

## CPU.NZCV.CINC.001

Entry symbol:

`nce_diag_raw_cpu_nzcv_cinc_001`

Invocation contract:

- `x0`: writable result buffer, at least `0x28` bytes
- no libc, libnx, heap, stack frame, TLS, SVC, or C runtime dependency
- incoming architectural `x18` is saved before the test and restored before return
- return value: `x0` again points to the same result buffer

Result layout:

| Offset | Field | Size |
| --- | --- | --- |
| `0x00` | `abi_version` | 4 |
| `0x04` | `input_w16` | 4 |
| `0x08` | `actual_result` | 4 |
| `0x0c` | `scratch_w17` | 4 |
| `0x10` | `scratch_w18` | 4 |
| `0x14` | reserved | 4 |
| `0x18` | `nzcv_before` | 8 |
| `0x20` | `nzcv_after` | 8 |

The `scratch_w17` field is intentionally at `+0x0c` so the core probe preserves the exact regression instruction shape:

```asm
cmp     w16, #0
mrs     x10, nzcv
stp     w17, w18, [x14, #0x0c]
mrs     x11, nzcv
mov     w2, #2
cinc    w2, w2, ne
```

For the current fixed input `w16 = 0`, the expected observable values are:

- `actual_result = 2`
- `nzcv_before = 0x60000000`
- `nzcv_after = 0x60000000`
- `scratch_w17 = 0x00001111`
- `scratch_w18 = 0x00002222`

The raw microtest records state only. PASS/FAIL classification belongs to the runner/harness so the measured instruction path stays minimal.
