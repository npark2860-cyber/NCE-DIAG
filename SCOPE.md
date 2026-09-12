# Scope

NCE-DIAG is an independent guest workload built with public devkitA64/libnx tooling.

Initial implementation order:

1. harness
2. `CPU.NZCV.CINC.001`
3. execution validation
4. additional CPU tests
5. IPC basics
6. moved-handle/domain tests
7. exception tests

Out of scope for the initial milestone: GPU, Vulkan, Adreno, shaders, audio, network, HID, filesystem stress, large multithreading stress, and performance benchmarking.
