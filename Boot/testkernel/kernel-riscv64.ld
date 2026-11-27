ENTRY(_start)
SECTIONS
{
	. = 0x80200000;
	.text : AT(0x80200000) { *(.text .text.*) }
	.rodata : { *(.rodata .rodata.*) }
	.data : { *(.data .data.*) *(.sdata .sdata.*) *(.got .got.*) }
	.bss (NOLOAD) : { *(COMMON) *(.bss .bss.*) *(.sbss .sbss.*) }
	/DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) *(.llvm*) *(.riscv.attributes) }
}
