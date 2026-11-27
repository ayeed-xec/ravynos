// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_RISCV64_TRAP_H
#define RV_RISCV64_TRAP_H

#define RISCV64_TRAP_FRAME_SLOTS 31
#define RISCV64_TRAP_FRAME_SIZE (RISCV64_TRAP_FRAME_SLOTS * 8)

struct riscv64_trap_frame {
    unsigned long tf_ra;
    unsigned long tf_gp;
    unsigned long tf_tp;
    unsigned long tf_t0;
    unsigned long tf_t1;
    unsigned long tf_t2;
    unsigned long tf_s0;
    unsigned long tf_s1;
    unsigned long tf_a0;
    unsigned long tf_a1;
    unsigned long tf_a2;
    unsigned long tf_a3;
    unsigned long tf_a4;
    unsigned long tf_a5;
    unsigned long tf_a6;
    unsigned long tf_a7;
    unsigned long tf_s2;
    unsigned long tf_s3;
    unsigned long tf_s4;
    unsigned long tf_s5;
    unsigned long tf_s6;
    unsigned long tf_s7;
    unsigned long tf_s8;
    unsigned long tf_s9;
    unsigned long tf_s10;
    unsigned long tf_s11;
    unsigned long tf_t3;
    unsigned long tf_t4;
    unsigned long tf_t5;
    unsigned long tf_t6;
    unsigned long tf_sp;
};

void riscv64_trap_entry(unsigned long scause, unsigned long sepc, unsigned long stval,
                        struct riscv64_trap_frame *frame);

#endif
