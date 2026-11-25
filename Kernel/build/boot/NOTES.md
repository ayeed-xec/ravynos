# WP-A2c boot harness — QEMU bring-up chain

Chosen chain: GRUB 2.12 (multiboot2) -> 32-bit ELF loader payload. The ELF is
the harness stand-in for the real `guest_kernel` Mach-O; the drop-in point is
`testimg/isodir/boot/kernel.elf` (see "Real kernel" below).

Exact commands (see `build.sh`, `qemu-boot.sh`):

	clang -m32 -ffreestanding -nostdlib (boot32.S + stub.c) -> kernel.elf
	grub-mkrescue -o testimg/test.iso testimg/isodir
	qemu-system-x86_64 -machine q35 -m 512 -display none -no-reboot \
		-cdrom testimg/test.iso -serial file:serial.log -daemonize

Captured serial (verified 2026-11-24, exit 0):

	RAVYNOS-BOOT-OK WP-A2c multiboot2 COM1
	grub magic=0x36d76289

Real kernel: QEMU cannot load a Mach-O directly (`-kernel` is ELF/multiboot
only). Two paths when WP-A2a/A2b deliver the real xnu `guest_kernel` Mach-O:
(a) extend the payload with a tiny ELF-loader stub that parses the Mach-O off
an attached disk and jumps to its LC_UNIXTHREAD entry, or (b) reuse the
Phase-1 ravynOS image-hook technique. Path (a) is planned; the GRUB chain
itself stays unchanged. Until then the multiboot2 payload proves the boot
chain, serial capture and banner detection end-to-end.

Rejected alternative: OVMF + hand-rolled UEFI loader — more code for the same
result at this stage; revisit if multiboot2 limits (32-bit entry) bite at
real-kernel drop-in, since xnu needs a 64-bit long-mode handoff either way.

Runtime artifacts (testimg/ except .gitignore) are generated, gitignored, and
not committed; GRUB is used as an external tool only, no GPL code committed.
