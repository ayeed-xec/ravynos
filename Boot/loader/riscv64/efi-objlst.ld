/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2026 The ravynOS Authors */
ENTRY(efi_main)
SECTIONS
{
	. = 0x1000;
	.text : { *(.text .text.*) }
	. = ALIGN(0x1000);
	.rodata : { *(.rodata .rodata.*) }
	. = ALIGN(0x1000);
	.data : { *(.data .data.*) *(.sdata .sdata.*) *(.got .got.*) }
	. = ALIGN(0x1000);
	.bss (NOLOAD) : { *(COMMON) *(.bss .bss.*) *(.sbss .sbss.*) }
	/DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) *(.llvm*) *(.riscv.attributes) *(.debug*) }
}
