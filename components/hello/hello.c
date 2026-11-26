// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <hello.h>

#if defined(__x86_64__)
static void sys_write(const char *msg, long len)
{
    __asm__ volatile("syscall" : : "a"(1L), "D"(1L), "S"(msg), "d"(len) : "rcx", "r11", "memory");
}

__attribute__((noreturn)) static void sys_exit(void)
{
    __asm__ volatile("syscall" ::"a"(60L), "D"(0L) : "rcx", "r11", "memory");
    __builtin_unreachable();
}
#elif defined(__aarch64__)
static void sys_write(const char *msg, long len)
{
    register long x8 __asm__("x8") = 64;
    register long x0 __asm__("x0") = 1;
    register long x1 __asm__("x1") = (long)msg;
    register long x2 __asm__("x2") = len;
    __asm__ volatile("svc 0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory");
}

__attribute__((noreturn)) static void sys_exit(void)
{
    register long x8 __asm__("x8") = 93;
    register long x0 __asm__("x0") = 0;
    __asm__ volatile("svc 0" ::"r"(x8), "r"(x0) : "memory");
    __builtin_unreachable();
}
#elif defined(__riscv)
static void sys_write(const char *msg, long len)
{
    register long a7 __asm__("a7") = 64;
    register long a0 __asm__("a0") = 1;
    register long a1 __asm__("a1") = (long)msg;
    register long a2 __asm__("a2") = len;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2) : "memory");
}

__attribute__((noreturn)) static void sys_exit(void)
{
    register long a7 __asm__("a7") = 93;
    register long a0 __asm__("a0") = 0;
    __asm__ volatile("ecall" ::"r"(a7), "r"(a0) : "memory");
    __builtin_unreachable();
}
#else
#error "hello: no syscall path for this architecture"
#endif

__attribute__((noreturn)) void _start(void)
{
    const char *msg = hello_message();
    long len = 0;
    while (msg[len] != '\0')
        len++;
    sys_write(msg, len);
    sys_write("\n", 1);
    sys_exit();
}
