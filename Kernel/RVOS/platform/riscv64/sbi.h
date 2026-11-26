// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_RISCV64_SBI_H
#define RV_RISCV64_SBI_H

#define RISCV64_SBI_BASE_EXT 0x10
#define RISCV64_SBI_BASE_GET_SPEC_VERSION 0
#define RISCV64_SBI_SRST_EXT 0x53525354UL
#define RISCV64_SBI_SRST_RESET 0
#define RISCV64_SBI_ERR_SUCCESS 0
#define RISCV64_SBI_ERR_NOT_SUPPORTED (-1)
#define RISCV64_SBI_ERR_DENIED (-2)
#define RISCV64_SBI_ERR_INVALID_PARAM (-3)

struct riscv64_sbiret {
    long error;
    long value;
};

void riscv64_sbi_console_putchar(int ch);
void riscv64_sbi_shutdown(void);
unsigned long riscv64_sbi_get_spec_version(void);

#endif
