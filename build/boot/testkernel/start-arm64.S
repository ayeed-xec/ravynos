// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
	.section	.bss.stack,"aw",@nobits
	.align	16
rv_stack_bottom:
	.space	0x10000
rv_stack_top:

	.text
	.globl	_start
_start:
	mov	x29, #0
	adrp	x16, rv_stack_top
	add	x16, x16, :lo12:rv_stack_top
	mov	sp, x16
	bl	kmain
.Lrv_halt:
	wfe
	b	.Lrv_halt
