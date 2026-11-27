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
	.option	push
	.option	norelax
	la	sp, rv_stack_top
	.option	pop
	tail	kmain
.Lrv_halt:
	wfi
	j	.Lrv_halt
