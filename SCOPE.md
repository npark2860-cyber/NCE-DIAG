# Scope

NCE-DIAG is an independent internal guest workload for Windows ARM64 NCE conformance and regression diagnosis.

Primary target semantics:

1. CPU architectural state
2. SVC boundaries
3. HIPC/session/handle semantics
4. exception/resume behavior
5. memory mapping/permission/boundary behavior

The project is not limited to public homebrew abstractions. Public devkitA64/libnx/switchbrew sources may be used, and user-authorized internal reference material may also be used when necessary, including firmware/title dumps, NSO/NPDM/ExeFS, ABI/API material, binaries/disassembly/logs, known-good captures, emulator implementations, and real-hardware results.

The initial bootstrap remains deliberately small:

1. harness
2. `CPU.NZCV.CINC.001`
3. build validation
4. one known-good runtime validation
5. Windows ARM64 Eden NCE validation
6. additional CPU tests
7. IPC/SVC tests
8. exception tests
9. memory tests

Out of scope for the initial milestone: GPU, Vulkan, Adreno, shaders, audio, network, HID, filesystem stress, large multithreading stress, and performance benchmarking.

A failure produced by NCE-DIAG is recorded and minimized here. Eden/Strato production fixes remain separate work.
