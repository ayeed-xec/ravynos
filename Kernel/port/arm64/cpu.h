// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

#ifndef _RVOS_ARM64_CPU_H_
#define _RVOS_ARM64_CPU_H_

#include "asm.h"

#define RV_ARM64_MAX_CPUS 8

struct rv_cpu_data {
    rv_u64 cd_cpu_number;
    rv_u64 cd_tpidr_el1;
    rv_u64 cd_ttbr0_el1;
    rv_u64 cd_vbar_el1;
    void *cd_active_pmap;
    void *cd_active_thread;
};

extern struct rv_cpu_data rv_cpu_data[RV_ARM64_MAX_CPUS];

static inline struct rv_cpu_data *rv_cpu_data_get(rv_u64 cpu_number)
{
    return &rv_cpu_data[cpu_number];
}

#endif
