// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
	.text
	.globl	rv_elf_handoff
rv_elf_handoff:
	movq	%rcx, %rax
	movq	%rdx, %rdi
	jmp	*%rax

	.globl	rv_macho_handoff
rv_macho_handoff:
	cli
	movq	%rcx, %rdi
	movq	%rdx, %rbx
	lgdt	rv_gdt_desc(%rip)
	leaq	.Lrv_compat32(%rip), %rcx
	pushq	$0x18
	pushq	%rcx
	lretq

	.code32
.Lrv_compat32:
	movl	%ebx, %eax
	movl	%cr0, %ecx
	andl	$0x7fffffff, %ecx
	movl	%ecx, %cr0
	ljmp	$0x18, $.Lrv_go
.Lrv_go:
	movl	$0x20, %ecx
	movl	%ecx, %ds
	movl	%ecx, %es
	movl	%ecx, %ss
	xorl	%ecx, %ecx
	movl	%ecx, %fs
	movl	%ecx, %gs
	jmp	*%edi

	.align	8
rv_gdt:
	.quad	0
	.quad	0x00af9b000000ffff
	.quad	0x00cf93000000ffff
	.quad	0x00cf9b000000ffff
	.quad	0x00cf93000000ffff
rv_gdt_end:
rv_gdt_desc:
	.word	rv_gdt_end - rv_gdt - 1
	.quad	rv_gdt
