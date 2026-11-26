// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
	.text
	.globl	rv_macho_handoff
	.globl	rv_elf_handoff
rv_macho_handoff:
rv_elf_handoff:
	mov	x3, x0
	mov	x0, x1
	mov	x1, x3
	ic	iallu
	dsb	ish
	isb
	br	x3
