// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

#if defined(__x86_64__)
__attribute__((noreturn)) static void sys_exit(void)
{
    __asm__ volatile("syscall" ::"a"(60L), "D"(0L) : "rcx", "r11", "memory");
    __builtin_unreachable();
}
#elif defined(__aarch64__)
__attribute__((noreturn)) static void sys_exit(void)
{
    register long x8 __asm__("x8") = 93;
    register long x0 __asm__("x0") = 0;
    __asm__ volatile("svc 0" ::"r"(x8), "r"(x0) : "memory");
    __builtin_unreachable();
}
#elif defined(__riscv)
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
    sys_exit();
}
