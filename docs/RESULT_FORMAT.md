# Result format

Primary path:

`sdmc:/nce_diag_result.json`

Fallback path:

`nce_diag_result.json`

Schema for 0.1:

```json
{
  "suite": "NCE-DIAG",
  "version": "0.1.0",
  "pass": 4,
  "fail": 1,
  "skip": 0,
  "tests": [
    {
      "id": "CPU.NZCV.CINC.001",
      "status": "PASS",
      "expected": 2,
      "actual": 2,
      "result_code": "0x00000000",
      "nzcv_before": "0x60000000",
      "nzcv_after": "0x60000000",
      "detail": "embedded raw cmp/stp-x18/cinc path"
    }
  ]
}
```

The file is overwritten on each 0.1 run. Timestamped run history can be added after the first runtime baseline is established.
