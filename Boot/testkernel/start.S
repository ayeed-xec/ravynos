// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
	.section	.bss.stack,"aw",@nobits
	.align	16
rv_stack_bottom:
	.space	0x10000
rv_stack_top:

	.text
	.globl	_start
_start:
	xorl	%ebp, %ebp
	movq	$rv_stack_top, %rsp
	call	kmain
.Lrv_halt:
	cli
	hlt
	jmp	.Lrv_halt
