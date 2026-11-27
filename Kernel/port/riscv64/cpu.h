// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_RISCV64_CPU_H
#define RV_RISCV64_CPU_H

#include "pmap.h"

struct riscv64_trap_frame;

struct riscv64_cpu_data {
    unsigned long cd_hartid;
    int cd_cpu_number;
    int cd_cpu_active;
    struct riscv64_pmap *cd_current_pmap;
    struct riscv64_trap_frame *cd_trap_frame;
    unsigned long cd_kernel_stack_top;
};

extern struct riscv64_cpu_data riscv64_cpu_data_master;

static inline struct riscv64_cpu_data *riscv64_cpu_data(int cpu_number)
{
    if (cpu_number == 0) {
        return &riscv64_cpu_data_master;
    }
    return (struct riscv64_cpu_data *)0;
}

#endif
