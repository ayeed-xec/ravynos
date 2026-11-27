// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdint.h>

#define RV_PL011_BASE 0x9000000ull

#define RV_PL011_DR 0x00
#define RV_PL011_FR 0x18
#define RV_PL011_IBRD 0x24
#define RV_PL011_FBRD 0x28
#define RV_PL011_LCR_H 0x2c
#define RV_PL011_CR 0x30

static inline void mmio_write32(uint64_t addr, uint32_t value)
{
	*(volatile uint32_t *)(uintptr_t)addr = value;
}

static inline uint32_t mmio_read32(uint64_t addr)
{
	return *(volatile uint32_t *)(uintptr_t)addr;
}

void rv_serial_init(void)
{
	mmio_write32(RV_PL011_BASE + RV_PL011_CR, 0);
	mmio_write32(RV_PL011_BASE + RV_PL011_IBRD, 13);
	mmio_write32(RV_PL011_BASE + RV_PL011_FBRD, 37);
	mmio_write32(RV_PL011_BASE + RV_PL011_LCR_H, (3u << 5) | (1u << 4));
	mmio_write32(RV_PL011_BASE + RV_PL011_CR, (1u << 0) | (1u << 8) | (1u << 9));
}

void rv_serial_putc(char c)
{
	while (mmio_read32(RV_PL011_BASE + RV_PL011_FR) & (1u << 5))
		;
	mmio_write32(RV_PL011_BASE + RV_PL011_DR, (uint8_t)c);
}

void rv_serial_puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			rv_serial_putc('\r');
		rv_serial_putc(*s++);
	}
}

void rv_serial_hex16(uint32_t value)
{
	static const char digits[] = "0123456789abcdef";
	rv_serial_puts("0x");
	for (int i = 28; i >= 0; i -= 4)
		rv_serial_putc(digits[(value >> i) & 0xf]);
}

void rv_serial_hex64(uint64_t value)
{
	static const char digits[] = "0123456789abcdef";
	rv_serial_puts("0x");
	for (int i = 60; i >= 0; i -= 4)
		rv_serial_putc(digits[(value >> i) & 0xf]);
}

void rv_memset(void *dst, uint8_t value, uint64_t size)
{
	uint8_t *d = dst;
	while (size--)
		*d++ = value;
}

void rv_memcpy(void *dst, const void *src, uint64_t size)
{
	uint8_t *d = dst;
	const uint8_t *s = src;
	while (size--)
		*d++ = *s++;
}

int rv_streq(const char *a, const char *b)
{
	while (*a && *b && *a == *b) {
		a++;
		b++;
	}
	return *a == *b;
}

uint64_t rv_strlen(const char *s)
{
	uint64_t n = 0;
	while (s[n])
		n++;
	return n;
}
