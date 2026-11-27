// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include "pmap.h"
#include "asm.h"

unsigned long riscv64_pmap_satp(const struct riscv64_pmap *pmap)
{
    unsigned long ppn = (pmap->pm_root_paddr[RISCV64_SV39_LEVELS - 1] >> RISCV64_PAGE_SHIFT) &
                        RISCV64_SATP_PPN_MASK;
    return RISCV64_SATP_MODE_SV39 | ppn;
}

void riscv64_pmap_init(struct riscv64_pmap *pmap, unsigned long root_paddr, unsigned long stage)
{
    unsigned long level;

    pmap->pm_satp = 0;
    pmap->pm_asid = 0;
    pmap->pm_stage = stage;
    pmap->pm_ref_count = 1;
    if (stage == RISCV64_PMAP_STAGE_USER) {
        pmap->pm_min_vaddr = 0;
        pmap->pm_max_vaddr = RISCV64_SV39_USER_TOP;
    }
    else {
        pmap->pm_min_vaddr = RISCV64_SV39_USER_TOP;
        pmap->pm_max_vaddr = ~0UL;
    }
    for (level = 0; level < RISCV64_SV39_LEVELS; level++) {
        pmap->pm_root_paddr[level] = 0;
    }
    pmap->pm_root_paddr[RISCV64_SV39_LEVELS - 1] = root_paddr & ~RISCV64_PAGE_MASK;
    pmap->pm_satp = riscv64_pmap_satp(pmap);
}

void riscv64_pmap_activate(const struct riscv64_pmap *pmap)
{
    RV_CSRW(satp, pmap->pm_satp);
    RV_SFENCE_VMA(0, 0);
}

int riscv64_pmap_empty(const struct riscv64_pmap *pmap)
{
    return pmap->pm_satp == 0;
}
