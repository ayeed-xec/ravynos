// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

#ifndef _RVOS_ARM64_PMAP_H_
#define _RVOS_ARM64_PMAP_H_

typedef unsigned long vm_offset_t;
typedef unsigned long vm_map_address_t;
typedef unsigned long vm_map_size_t;
typedef unsigned long pmap_paddr_t;
typedef unsigned long addr64_t;
typedef unsigned int ppnum_t;
typedef unsigned int vm_prot_t;
typedef int kern_return_t;
typedef int boolean_t;

typedef unsigned long long rv_pte_t;
typedef struct rv_pmap *pmap_t;

#define KERN_SUCCESS 0
#define KERN_FAILURE 5
#define KERN_NO_SPACE 3
#define KERN_RESOURCE_SHORTAGE 6

#define VM_PROT_NONE 0x00
#define VM_PROT_READ 0x01
#define VM_PROT_WRITE 0x02
#define VM_PROT_EXECUTE 0x04

#define FALSE 0
#define TRUE 1

#define RV_ARM64_PAGE_SHIFT 12
#define RV_ARM64_PAGE_SIZE 0x1000UL
#define RV_ARM64_GRANULE 0x1000UL
#define RV_ARM64_VA_BITS 48
#define RV_ARM64_T0SZ 16
#define RV_ARM64_TTBR_ADDR_BITS 48
#define RV_ARM64_PTES_PER_TABLE 512
#define RV_ARM64_PT_LEVELS 4
#define RV_ARM64_L0_SIZE (RV_ARM64_PAGE_SIZE)

#define RV_ARM64_PTE_VALID 0x1ULL
#define RV_ARM64_PTE_TYPE_BLOCK 0x0ULL
#define RV_ARM64_PTE_TYPE_PAGE 0x2ULL
#define RV_ARM64_PTE_TYPE_TABLE 0x3ULL
#define RV_ARM64_PTE_TABLE_AF 0x400ULL
#define RV_ARM64_PTE_ATTRINDX_SHIFT 2
#define RV_ARM64_PTE_ATTRINDX_MASK 0x7ULL
#define RV_ARM64_PTE_NS 0x200ULL
#define RV_ARM64_PTE_AP_RO_EL1 0x0ULL
#define RV_ARM64_PTE_AP_RW_EL1 0x400ULL
#define RV_ARM64_PTE_AP_RO_ALL 0x600ULL
#define RV_ARM64_PTE_AP_RW_ALL 0x200ULL
#define RV_ARM64_PTE_SH_SHIFT 8
#define RV_ARM64_PTE_SH_INNER 0x3ULL
#define RV_ARM64_PTE_SH_OUTER 0x2ULL
#define RV_ARM64_PTE_AF 0x400ULL
#define RV_ARM64_PTE_PXN 0x8000000000000000ULL
#define RV_ARM64_PTE_UXN 0x4000000000000000ULL
#define RV_ARM64_PTE_CONT_SHIFT 52
#define RV_ARM64_PTE_NG 0x80000000000000ULL

#define RV_ARM64_ATTRINDX_NORMAL 0
#define RV_ARM64_ATTRINDX_DEVICE 1
#define RV_ARM64_ATTRINDX_NORMAL_NC 2

#define RV_ARM64_MAIR_NORMAL_WB 0xffUL
#define RV_ARM64_MAIR_DEVICE_nGnRE 0x04UL
#define RV_ARM64_MAIR_NORMAL_NC 0x44UL

#define RV_ARM64_PTE_ATTR(indx)                                                                    \
    (((rv_pte_t)(indx) & RV_ARM64_PTE_ATTRINDX_MASK) << RV_ARM64_PTE_ATTRINDX_SHIFT)

struct rv_pmap {
    rv_pte_t *pm_l0;
    pmap_paddr_t pm_l0_phys;
};

#define RV_PMAP_INVALID ((pmap_t)0)

extern void pmap_bootstrap(vm_offset_t vstart);
extern void pmap_init(void);
extern vm_map_address_t pmap_map(vm_map_address_t virt, vm_offset_t start, vm_offset_t end,
                                 vm_prot_t prot, unsigned int flags);
extern vm_map_address_t pmap_map_bd(vm_map_address_t virt, vm_offset_t start, vm_offset_t end,
                                    vm_prot_t prot);
extern kern_return_t pmap_map_block(pmap_t pmap, addr64_t va, ppnum_t pa, unsigned int size,
                                    vm_prot_t prot, int attr, unsigned int flags);
extern kern_return_t pmap_enter_options_addr(pmap_t pmap, vm_map_address_t v, pmap_paddr_t pa,
                                             vm_prot_t prot, vm_prot_t fault_type,
                                             unsigned int flags, boolean_t wired,
                                             unsigned int options, void *arg,
                                             unsigned int mapping_type);
extern void pmap_remove(pmap_t pmap, vm_map_address_t start, vm_map_address_t end);
extern void pmap_protect(pmap_t pmap, vm_map_address_t b, vm_map_address_t e, vm_prot_t prot);
extern void pmap_page_protect(ppnum_t ppnum, vm_prot_t prot);
extern void pmap_switch(pmap_t pmap, void *thread);
extern void pmap_clear_user_ttb(void);
extern pmap_paddr_t kvtophys(vm_offset_t va);
extern vm_map_address_t phystokv(pmap_paddr_t pa);
extern ppnum_t pmap_find_phys(pmap_t pmap, addr64_t va);
extern boolean_t pmap_valid_address(pmap_paddr_t addr);

#endif
