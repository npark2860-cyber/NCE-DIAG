.section .text, "ax", %progbits
.align 2
.include "../include/cpu_nzcv_cinc_core.inc"

.global nce_diag_raw_cpu_nzcv_cinc_001
.type nce_diag_raw_cpu_nzcv_cinc_001, %function

// Executable-embedded raw microtest ABI:
//   x0 = writable NceDiagCpuNzcvCincRawResult buffer (0x28 bytes minimum)
//   return = x0 preserved as the result-buffer pointer
nce_diag_raw_cpu_nzcv_cinc_001:
    mov     x14, x0
    mov     x15, x18

    mov     w16, #0
    mov     w17, #0x1111
    mov     w18, #0x2222

    NCE_DIAG_CPU_NZCV_CINC_CORE x14, x10, x11, w2

    mov     w3, #1
    str     w3, [x14, #0x00]
    mov     w3, #0
    str     w3, [x14, #0x04]
    str     w2, [x14, #0x08]
    str     x10, [x14, #0x18]
    str     x11, [x14, #0x20]

    mov     x18, x15
    mov     x0, x14
    ret
.size nce_diag_raw_cpu_nzcv_cinc_001, .-nce_diag_raw_cpu_nzcv_cinc_001
