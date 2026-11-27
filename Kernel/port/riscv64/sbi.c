// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include "sbi.h"

static struct riscv64_sbiret riscv64_sbi_ecall(unsigned long ext, unsigned long fid,
                                               unsigned long arg0, unsigned long arg1,
                                               unsigned long arg2)
{
    struct riscv64_sbiret ret;
    register unsigned long a0 __asm__("a0") = arg0;
    register unsigned long a1 __asm__("a1") = arg1;
    register unsigned long a2 __asm__("a2") = arg2;
    register unsigned long a6 __asm__("a6") = fid;
    register unsigned long a7 __asm__("a7") = ext;

    __asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a6), "r"(a7) : "memory");
    ret.error = (long)a0;
    ret.value = (long)a1;
    return ret;
}

void riscv64_sbi_console_putchar(int ch)
{
    riscv64_sbi_ecall(0x01, 0, (unsigned long)ch, 0, 0);
}

void riscv64_sbi_shutdown(void)
{
    riscv64_sbi_ecall(RISCV64_SBI_SRST_EXT, RISCV64_SBI_SRST_RESET, 0, 0, 0);
}

unsigned long riscv64_sbi_get_spec_version(void)
{
    struct riscv64_sbiret ret;

    ret = riscv64_sbi_ecall(RISCV64_SBI_BASE_EXT, RISCV64_SBI_BASE_GET_SPEC_VERSION, 0, 0, 0);
    if (ret.error != RISCV64_SBI_ERR_SUCCESS) {
        return 0;
    }
    return ret.value;
}
