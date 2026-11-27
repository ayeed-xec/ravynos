// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_RISCV64_ASM_H
#define RV_RISCV64_ASM_H

#define RV_CSRR(name)                                                                              \
    ({                                                                                             \
        unsigned long rv_csrr_value_;                                                              \
        __asm__ volatile("csrr %0, " #name : "=r"(rv_csrr_value_)::);                              \
        rv_csrr_value_;                                                                            \
    })

#define RV_CSRW(name, value)                                                                       \
    do {                                                                                           \
        unsigned long rv_csrw_value_ = (value);                                                    \
        __asm__ volatile("csrw " #name ", %0" ::"r"(rv_csrw_value_) :);                            \
    } while (0)

#define RV_CSRS(name, bits)                                                                        \
    do {                                                                                           \
        unsigned long rv_csrs_bits_ = (bits);                                                      \
        __asm__ volatile("csrs " #name ", %0" ::"r"(rv_csrs_bits_) :);                             \
    } while (0)

#define RV_CSRC(name, bits)                                                                        \
    do {                                                                                           \
        unsigned long rv_csrc_bits_ = (bits);                                                      \
        __asm__ volatile("csrc " #name ", %0" ::"r"(rv_csrc_bits_) :);                             \
    } while (0)

#define RV_CSR_SWAP(name, value)                                                                   \
    ({                                                                                             \
        unsigned long rv_csrw_old_ = (value);                                                      \
        __asm__ volatile("csrrw %0, " #name ", %1" : "=&r"(rv_csrw_old_) : "r"(rv_csrw_old_) :);   \
        rv_csrw_old_;                                                                              \
    })

#define RV_FENCE_I() __asm__ volatile("fence.i" ::: "memory")
#define RV_SFENCE_VMA(asid, vaddr)                                                                 \
    do {                                                                                           \
        unsigned long rv_sfence_asid_ = (asid);                                                    \
        unsigned long rv_sfence_vaddr_ = (vaddr);                                                  \
        __asm__ volatile("sfence.vma %0, %1" ::"r"(rv_sfence_vaddr_), "r"(rv_sfence_asid_)         \
                         : "memory");                                                              \
    } while (0)

#endif
