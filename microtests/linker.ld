OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(nce_diag_raw_cpu_nzcv_cinc_001)

SECTIONS
{
    . = 0;

    .text : ALIGN(16)
    {
        KEEP(*(.text.entry))
        *(.text*)
    }

    /DISCARD/ :
    {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
    }
}
