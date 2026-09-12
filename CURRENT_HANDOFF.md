# CURRENT_HANDOFF

Branch: `feat/nce-diag-0.1-minimal`

Project role: private/internal guest-side conformance and diagnostic bench for Windows ARM64 NCE. Eden/Strato production code remains read-only from this project.

Current scope remains exactly one semantic: `CPU.NZCV.CINC.001`.

Implemented layers:

- standalone raw ARM64 microtest
- devkitA64/libnx NRO bootstrap runner
- shared assembly macro for the measured core
- fixed raw result ABI with compile-time offset/size assertions
- compiled instruction-order verification
- CI artifact production for both raw and NRO forms

Validated build baseline:

- implementation/docs SHA: `564fc12b7389f672cb50f0137c01f4183b061784`
- workflow run: `34676469257` — PASS
- NRO artifact `10292192995` — `NCE-DIAG-nro`
- raw artifact `10292208003` — `NCE-DIAG-raw-CPU-NZCV-CINC-001`
- raw binary size guard: PASS (`<= 256` bytes)
- compiled instruction sequence guard: PASS
- raw ABI `_Static_assert` compile-check: PASS

Expected semantic for fixed input `w16=0`:

- `actual_result = 2`
- `nzcv_before = 0x60000000`
- `nzcv_after = 0x60000000`

Runtime validation is still pending. Do not add `CPU.NZCV.CSEL.001` yet.

Next runtime order:

1. execute the current semantic in one known-good reference environment;
2. execute it in Windows ARM64 Eden NCE;
3. compare result/NZCV values;
4. record the differential result;
5. only then advance to the next semantic.
