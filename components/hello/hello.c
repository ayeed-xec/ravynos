// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <hello.h>

static void write_out(const char *msg, long len)
{
    __asm__ volatile("syscall" : : "a"(1L), "D"(1L), "S"(msg), "d"(len) : "rcx", "r11", "memory");
}

__attribute__((noreturn)) void _start(void)
{
    const char *msg = hello_message();
    long len = 0;
    while (msg[len] != '\0')
        len++;
    write_out(msg, len);
    write_out("\n", 1);
    __asm__ volatile("syscall" ::"a"(60L), "D"(0L) : "rcx", "r11", "memory");
    __builtin_unreachable();
}
