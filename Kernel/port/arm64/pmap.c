// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

#include "pmap.h"

void pmap_bootstrap(vm_offset_t vstart)
{
    (void)vstart;
}

void pmap_init(void) {}

vm_map_address_t pmap_map(vm_map_address_t virt, vm_offset_t start, vm_offset_t end, vm_prot_t prot,
                          unsigned int flags)
{
    (void)virt;
    (void)start;
    (void)end;
    (void)prot;
    (void)flags;
    return 0;
}

vm_map_address_t pmap_map_bd(vm_map_address_t virt, vm_offset_t start, vm_offset_t end,
                             vm_prot_t prot)
{
    (void)virt;
    (void)start;
    (void)end;
    (void)prot;
    return 0;
}

kern_return_t pmap_map_block(pmap_t pmap, addr64_t va, ppnum_t pa, unsigned int size,
                             vm_prot_t prot, int attr, unsigned int flags)
{
    (void)pmap;
    (void)va;
    (void)pa;
    (void)size;
    (void)prot;
    (void)attr;
    (void)flags;
    return KERN_SUCCESS;
}

kern_return_t pmap_enter_options_addr(pmap_t pmap, vm_map_address_t v, pmap_paddr_t pa,
                                      vm_prot_t prot, vm_prot_t fault_type, unsigned int flags,
                                      boolean_t wired, unsigned int options, void *arg,
                                      unsigned int mapping_type)
{
    (void)pmap;
    (void)v;
    (void)pa;
    (void)prot;
    (void)fault_type;
    (void)flags;
    (void)wired;
    (void)options;
    (void)arg;
    (void)mapping_type;
    return KERN_SUCCESS;
}

void pmap_remove(pmap_t pmap, vm_map_address_t start, vm_map_address_t end)
{
    (void)pmap;
    (void)start;
    (void)end;
}

void pmap_protect(pmap_t pmap, vm_map_address_t b, vm_map_address_t e, vm_prot_t prot)
{
    (void)pmap;
    (void)b;
    (void)e;
    (void)prot;
}

void pmap_page_protect(ppnum_t ppnum, vm_prot_t prot)
{
    (void)ppnum;
    (void)prot;
}

void pmap_switch(pmap_t pmap, void *thread)
{
    (void)pmap;
    (void)thread;
}

void pmap_clear_user_ttb(void) {}

pmap_paddr_t kvtophys(vm_offset_t va)
{
    (void)va;
    return 0;
}

vm_map_address_t phystokv(pmap_paddr_t pa)
{
    (void)pa;
    return 0;
}

ppnum_t pmap_find_phys(pmap_t pmap, addr64_t va)
{
    (void)pmap;
    (void)va;
    return 0;
}

boolean_t pmap_valid_address(pmap_paddr_t addr)
{
    (void)addr;
    return FALSE;
}
