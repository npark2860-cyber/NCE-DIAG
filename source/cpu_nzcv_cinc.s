.text
.align 2
.include "../include/cpu_nzcv_cinc_core.inc"
.global nce_diag_cpu_nzcv_cinc_probe
.type nce_diag_cpu_nzcv_cinc_probe, %function
nce_diag_cpu_nzcv_cinc_probe:
    // AAPCS64: w0=input, x1=scratch, x2=&nzcv_before, x3=&nzcv_after
    // Preserve incoming x18 so the probe is safe to call from the C harness.
    mov     x12, x18
    mov     w16, w0
    mov     x0, x1
    mov     x8, x2
    mov     x9, x3
    mov     w17, #0x1111
    mov     w18, #0x2222

    NCE_DIAG_CPU_NZCV_CINC_CORE x0, x10, x11, w2

    str     x10, [x8]
    str     x11, [x9]
    mov     w0, w2
    mov     x18, x12
    ret
.size nce_diag_cpu_nzcv_cinc_probe, .-nce_diag_cpu_nzcv_cinc_probe
