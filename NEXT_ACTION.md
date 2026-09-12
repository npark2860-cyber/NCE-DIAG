# NEXT_ACTION

Do not add second-milestone tests yet.

The executable first-milestone candidate is built and packaged. Next action is runtime validation of the exact `NCE-DIAG.nro` with SHA-256:

`ed16d5872ca1b3ddab3aebd9388db448fb2b0b054047a1ea0941007f66c026d7`

Validation order:

1. launch the NRO in Eden;
2. confirm boot reaches `NCE-DIAG 0.1.0`;
3. capture all five PASS/FAIL lines;
4. confirm summary counts;
5. confirm `nce_diag_result.json` is created and parseable;
6. confirm the application exits normally without input;
7. preserve the result file/log as the first Windows ARM64 NCE runtime baseline.

If runtime fails, minimize and record the failing semantic here; do not modify Eden production implementation in this project.
