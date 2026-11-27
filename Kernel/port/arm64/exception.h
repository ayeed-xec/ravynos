// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors

#ifndef _RVOS_ARM64_EXCEPTION_H_
#define _RVOS_ARM64_EXCEPTION_H_

#include "asm.h"

#define RV_ARM64_VECTOR_TABLE_SIZE 2048
#define RV_ARM64_VECTOR_ENTRY_SIZE 128
#define RV_ARM64_VECTOR_ENTRIES 16

struct rv_arm64_trap_frame {
    rv_u64 tf_x[30];
    rv_u64 tf_lr;
    rv_u64 tf_elr;
    rv_u64 tf_spsr;
    rv_u64 tf_esr;
};

_Static_assert(sizeof(struct rv_arm64_trap_frame) == 272, "trap frame layout mismatch");

extern unsigned char rv_arm64_vectors[RV_ARM64_VECTOR_TABLE_SIZE];

void arm64_exception_entry(struct rv_arm64_trap_frame *frame);

static inline void rv_arm64_vbar_el1_set(const void *vectors)
{
    RV_ARM64_MSR(vbar_el1, (rv_u64)vectors);
    __asm__ volatile("isb" ::: "memory");
}

#endif
