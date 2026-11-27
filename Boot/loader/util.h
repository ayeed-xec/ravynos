// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_LOADER_UTIL_H
#define RV_LOADER_UTIL_H

#include <stdint.h>

void rv_serial_init(void);
void rv_serial_putc(char c);
void rv_serial_puts(const char *s);
void rv_serial_hex16(uint32_t value);
void rv_serial_hex64(uint64_t value);
void rv_memset(void *dst, uint8_t value, uint64_t size);
void rv_memcpy(void *dst, const void *src, uint64_t size);
int rv_streq(const char *a, const char *b);
uint64_t rv_strlen(const char *s);

void rv_elf_handoff(uint64_t entry, uint64_t bootargs);
void rv_macho_handoff(uint64_t entry, uint64_t bootargs);

#endif
