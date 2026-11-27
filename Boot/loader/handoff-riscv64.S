// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
	.text
	.globl	rv_macho_handoff
	.globl	rv_elf_handoff
rv_macho_handoff:
rv_elf_handoff:
	mv	t2, a0
	mv	a0, a1
	mv	a1, t2
	fence.i
	jalr	t2
