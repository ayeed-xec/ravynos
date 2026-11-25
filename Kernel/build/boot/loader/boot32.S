    .section .text.boot, "ax"
    .globl _start
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kmain
    cli
1:  hlt
    jmp 1b
