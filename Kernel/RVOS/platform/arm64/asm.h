// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors

#ifndef _RVOS_ARM64_ASM_H_
#define _RVOS_ARM64_ASM_H_

typedef unsigned long rv_u64;
typedef unsigned int rv_u32;

#define RV_ARM64_MSR(reg, value)                                                                   \
    __asm__ volatile("msr " #reg ", %0" : : "r"((rv_u64)(value)) : "memory")

#define RV_ARM64_MRS(reg, outvar) __asm__ volatile("mrs %0, " #reg : "=r"(outvar))

static inline void rv_arm64_isb(void)
{
    __asm__ volatile("isb" ::: "memory");
}

static inline void rv_arm64_dsb_sy(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
}

static inline void rv_arm64_dmb_sy(void)
{
    __asm__ volatile("dmb sy" ::: "memory");
}

static inline void rv_arm64_dmb_ish(void)
{
    __asm__ volatile("dmb ish" ::: "memory");
}

static inline void rv_arm64_dmb_ishst(void)
{
    __asm__ volatile("dmb ishst" ::: "memory");
}

static inline void rv_arm64_wfe(void)
{
    __asm__ volatile("wfe");
}

#endif
