# Build validation

The current implementation branch is validated in two layers:

1. local static validation of the AArch64 assembly and C harness;
2. devkitA64/libnx NRO build validation through GitHub Actions.

Runtime execution remains a separate step and is not inferred from a successful NRO build.
