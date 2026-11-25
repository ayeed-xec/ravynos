	.section .multiboot, "a"
	.code32
	.align 8
mb2_header:
	.long 0xE85250D6
	.long 0
	.long mb2_end - mb2_header
	.long -(0xE85250D6 + 0 + (mb2_end - mb2_header))
	.short 0
	.short 0
	.long 8
mb2_end:

	.section .gdt, "aw"
	.align 8
gdt:
	.quad 0
	.quad 0x00AF9A000000FFFF
gdt_desc32:
	.word 15
	.long gdt

	.section .bss
	.align 4096
pml4:
	.skip 4096
pml3_low:
	.skip 4096
pml3_high:
	.skip 4096
pdt:
	.skip 4096
pdt_high:
	.skip 4096
	.align 16
stack_bottom:
	.skip 65536
	.globl stack_top
stack_top:
saved_eax:
	.skip 4
saved_ebx:
	.skip 4

	.section .text
	.code32
	.globl _start
_start:
	cli
	mov	%eax, saved_eax
	mov	%ebx, saved_ebx
	mov	$stack_top, %esp
	call	serial_init32
	mov	$msg_boot, %esi
	call	puts32
	call	fill_tables
	mov	$pml4, %eax
	mov	%eax, %cr3
	mov	%cr4, %eax
	or	$0x20, %eax
	mov	%eax, %cr4
	mov	$0xC0000080, %ecx
	rdmsr
	or	$0x100, %eax
	wrmsr
	lgdt	gdt_desc32
	mov	%cr0, %eax
	or	$0x80010000, %eax
	mov	%eax, %cr0
	ljmp	$0x08, $long_entry

fill_tables:
	mov	$pdt, %edi
	mov	$0x83, %eax
	mov	$512, %ecx
1:	mov	%eax, (%edi)
	add	$0x200000, %eax
	add	$8, %edi
	loop	1b
	mov	$pml3_low, %edi
	mov	$pdt, %eax
	or	$3, %eax
	mov	%eax, (%edi)
	mov	$pml3_high, %edi
	mov	$pdt, %eax
	or	$3, %eax
	mov	%eax, 510*8(%edi)
	mov	$pml3_low, %edi
	mov	$pdt_high, %eax
	or	$3, %eax
	mov	%eax, 4*8(%edi)
	mov	$pdt_high, %edi
	mov	$0x83, %eax
	mov	%eax, (%edi)
	add	$0x200000, %eax
	mov	%eax, 8(%edi)
	mov	$pml4, %edi
	mov	$pml3_low, %eax
	or	$3, %eax
	mov	%eax, (%edi)
	mov	$pml3_high, %eax
	or	$3, %eax
	mov	%eax, 511*8(%edi)
	ret

serial_init32:
	mov	$0x3f9, %dx
	xor	%al, %al
	out	%al, %dx
	mov	$0x3fb, %dx
	mov	$0x80, %al
	out	%al, %dx
	mov	$0x3f8, %dx
	mov	$0x01, %al
	out	%al, %dx
	mov	$0x3f9, %dx
	xor	%al, %al
	out	%al, %dx
	mov	$0x3fb, %dx
	mov	$0x03, %al
	out	%al, %dx
	mov	$0x3fa, %dx
	mov	$0xC7, %al
	out	%al, %dx
	mov	$0x3fc, %dx
	mov	$0x0B, %al
	out	%al, %dx
	ret

puts32:
1:	lodsb
	test	%al, %al
	je	2f
	call	putc32
	jmp	1b
2:	ret

putc32:
	mov	%al, %bl
	mov	$0x3fd, %dx
3:	in	%dx, %al
	test	$0x20, %al
	je	3b
	mov	$0x3f8, %dx
	mov	%bl, %al
	out	%al, %dx
	ret

	.section .rodata
msg_boot:
	.asciz "RAVYNOS-BOOT-OK WP-A2 macho-loader long-mode\n"

	.code64
long_entry:
	mov	$stack_top, %rsp
	xor	%ebp, %ebp
	mov	saved_ebx, %edi
	mov	saved_eax, %esi
	call	c_entry
2:	cli
	hlt
	jmp	2b
