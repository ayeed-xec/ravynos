/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2026 The ravynOS Authors */
ENTRY(_start)
SECTIONS
{
	. = 0x200000;
	.text : { *(.text .text.*) }
	.rodata : { *(.rodata .rodata.*) }
	.data : { *(.data .data.*) }
	.bss.stack (NOLOAD) : { *(.bss.stack) }
	.bss (NOLOAD) : { *(COMMON) *(.bss .bss.*) }
	/DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) *(.llvm*) }
}
