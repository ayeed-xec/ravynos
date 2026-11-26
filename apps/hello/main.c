// SPDX-License-Identifier: BSD-3-Clause

void _start(void) {
    volatile unsigned long long exit_code = 0;
    __asm__ volatile("mov $60, %%rax\nmov %%rbx, %%rdi\nsyscall" :: : "rax", "rdi");
    (void)exit_code;
}
