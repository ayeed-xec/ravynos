// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdint.h>

#define RV_UART8250_BASE 0x10000000ull

#define RV_UART_THR 0x00
#define RV_UART_LSR 0x05
#define RV_UART_LSR_THRE 0x20ull
#define RV_UART_IER 0x01
#define RV_UART_FCR 0x02
#define RV_UART_LCR 0x03
#define RV_UART_MCR 0x04
#define RV_UART_LCR_8N1 0x03

static inline void mmio_write8(uint64_t addr, uint8_t value)
{
    *(volatile uint8_t *)(uintptr_t)addr = value;
}

static inline uint8_t mmio_read8(uint64_t addr)
{
    return *(volatile uint8_t *)(uintptr_t)addr;
}

void rv_serial_init(void)
{
    mmio_write8(RV_UART8250_BASE + RV_UART_IER, 0);
    mmio_write8(RV_UART8250_BASE + RV_UART_LCR, RV_UART_LCR_8N1);
    mmio_write8(RV_UART8250_BASE + RV_UART_FCR, 0x07);
    mmio_write8(RV_UART8250_BASE + RV_UART_MCR, 0x0b);
}

void rv_serial_putc(char c)
{
    while (!(mmio_read8(RV_UART8250_BASE + RV_UART_LSR) & RV_UART_LSR_THRE))
        ;
    mmio_write8(RV_UART8250_BASE + RV_UART_THR, (uint8_t)c);
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
