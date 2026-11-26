// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_RISCV64_PMAP_H
#define RV_RISCV64_PMAP_H

#define RISCV64_PAGE_SHIFT 12
#define RISCV64_PAGE_SIZE (1UL << RISCV64_PAGE_SHIFT)
#define RISCV64_PAGE_MASK (RISCV64_PAGE_SIZE - 1)

#define RISCV64_SV39_VA_BITS 39
#define RISCV64_SV39_LEVELS 3
#define RISCV64_SV39_PT_ENTRIES 512
#define RISCV64_SV39_PTE_SIZE 8
#define RISCV64_SV39_PPN_SHIFT 10
#define RISCV64_SV39_VPN_SHIFT(level) (RISCV64_PAGE_SHIFT + 9 * (level))
#define RISCV64_SV39_USER_TOP (1UL << RISCV64_SV39_VA_BITS)

#define RISCV64_PTE_V (1UL << 0)
#define RISCV64_PTE_R (1UL << 1)
#define RISCV64_PTE_W (1UL << 2)
#define RISCV64_PTE_X (1UL << 3)
#define RISCV64_PTE_U (1UL << 4)
#define RISCV64_PTE_G (1UL << 5)
#define RISCV64_PTE_A (1UL << 6)
#define RISCV64_PTE_D (1UL << 7)
#define RISCV64_PTE_PPN_MASK (((1UL << 44) - 1) << RISCV64_SV39_PPN_SHIFT)

#define RISCV64_SATP_MODE_SV39 (8UL << 60)
#define RISCV64_SATP_ASID_SHIFT 44
#define RISCV64_SATP_ASID_MASK (0xFFFFUL << RISCV64_SATP_ASID_SHIFT)
#define RISCV64_SATP_PPN_MASK ((1UL << 44) - 1)

#define RISCV64_HGATP_MODE_SV39X4 (8UL << 60)
#define RISCV64_HGATP_VMID_SHIFT 44
#define RISCV64_HGATP_VMID_MASK (0x3FFFUL << RISCV64_HGATP_VMID_SHIFT)
#define RISCV64_HGATP_PPN_MASK ((1UL << 43) - 1)

#define RISCV64_PMAP_STAGE_USER 0
#define RISCV64_PMAP_STAGE_KERNEL 1

struct riscv64_pmap {
    unsigned long pm_satp;
    unsigned long pm_asid;
    unsigned long pm_root_paddr[RISCV64_SV39_LEVELS];
    unsigned long pm_stage;
    unsigned long pm_min_vaddr;
    unsigned long pm_max_vaddr;
    unsigned long pm_ref_count;
};

void riscv64_pmap_init(struct riscv64_pmap *pmap, unsigned long root_paddr, unsigned long stage);
unsigned long riscv64_pmap_satp(const struct riscv64_pmap *pmap);
void riscv64_pmap_activate(const struct riscv64_pmap *pmap);
int riscv64_pmap_empty(const struct riscv64_pmap *pmap);

#endif
