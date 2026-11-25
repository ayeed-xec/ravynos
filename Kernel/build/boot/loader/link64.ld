ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text : { *(.multiboot) *(.text .text.*) }
    .gdt : { *(.gdt) }
    .rodata : { *(.rodata .rodata.*) }
    .bss : { *(.bss .bss.*) *(COMMON) }
}
