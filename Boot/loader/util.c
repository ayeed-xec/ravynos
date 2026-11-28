// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

#define RV_SERIAL_PORT 0x3f8

void rv_serial_init(void)
{
    outb(RV_SERIAL_PORT + 1, 0x00);
    outb(RV_SERIAL_PORT + 3, 0x80);
    outb(RV_SERIAL_PORT, 0x01);
    outb(RV_SERIAL_PORT + 1, 0x00);
    outb(RV_SERIAL_PORT + 3, 0x03);
    outb(RV_SERIAL_PORT + 2, 0xc7);
    outb(RV_SERIAL_PORT + 4, 0x0b);
}

void rv_serial_putc(char c)
{
    while (!(inb(RV_SERIAL_PORT + 5) & 0x20))
        ;
    outb(RV_SERIAL_PORT, (uint8_t)c);
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
