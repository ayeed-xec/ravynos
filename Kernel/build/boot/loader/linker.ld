ENTRY(_start)

SECTIONS
{
    . = 1M;

    .mboot2 : { KEEP(*(.mboot2)) }
    .text : { *(.text.boot) *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(COMMON) *(.bss*) }

    . = ALIGN(16);
    stack_bottom = .;
    . += 16K;
    stack_top = .;
}
