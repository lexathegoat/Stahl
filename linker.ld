/* linker.ld - Kernel linker script */

ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text : AT(0x1000)
    {
        *(.text)
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}