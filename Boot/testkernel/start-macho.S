// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
	.text
	.code32
	.globl	_start
_start:
	cli
	movl	%eax, %edi

	movl	$rv_pdpt, %eax
	orl	$3, %eax
	movl	%eax, rv_pml4

	movl	$rv_pd, %eax
	orl	$3, %eax
	movl	%eax, rv_pdpt

	movl	$rv_pml4, %eax
	movl	%eax, %cr3

	movl	%cr4, %eax
	orl	$0x20, %eax
	movl	%eax, %cr4

	movl	$0xC0000080, %ecx
	rdmsr
	orl	$0x100, %eax
	wrmsr

	movl	%cr0, %eax
	orl	$0x80010000, %eax
	movl	%eax, %cr0

	ljmp	$0x08, $.Lrv_long

	.code64
.Lrv_long:
	leaq	rv_stack_top+0x10000(%rip), %rsp
	call	_kmain
	cli
	hlt
	jmp	.Lrv_long

	.balign	4096
rv_pml4:
	.quad	0
	.space	4088
	.balign	4096
rv_pdpt:
	.quad	0
	.space	4088
	.balign	4096
rv_pd:
	i = 0
	.rept	512
	.quad	i * 0x200000 + 0x83
	i = i + 1
	.endr

	.zerofill __DATA,__stack,rv_stack_top,0x10000,3
