#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD="$ROOT/build"
mkdir -p "$BUILD"

HOST_CFLAGS=(-O2 -Wall -Wextra)
if [[ ! -x "$BUILD/mkfat" || "$ROOT/mkfat/mkfat.c" -nt "$BUILD/mkfat" ]]; then
	clang "${HOST_CFLAGS[@]}" -o "$BUILD/mkfat" "$ROOT/mkfat/mkfat.c"
fi
if [[ ! -x "$BUILD/pefix" || "$ROOT/pefix/pefix.c" -nt "$BUILD/pefix" ]]; then
	clang "${HOST_CFLAGS[@]}" -o "$BUILD/pefix" "$ROOT/pefix/pefix.c"
fi

UEFI_TARGET=x86_64-pc-windows-gnu
UEFI_CFLAGS=(
	--target="$UEFI_TARGET"
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mno-red-zone
	-mno-mmx
	-mno-sse
	-mno-sse2
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

for src in main elf macho util; do
	clang "${UEFI_CFLAGS[@]}" -c "$ROOT/loader/$src.c" -o "$BUILD/loader-$src.obj"
done
clang --target="$UEFI_TARGET" -c "$ROOT/loader/handoff.S" -o "$BUILD/loader-handoff.obj"

needs_link=0
[[ ! -f "$BUILD/BOOTX64.EFI" ]] && needs_link=1
for obj in "$BUILD"/loader-*.obj; do
	[[ "$obj" -nt "$BUILD/BOOTX64.EFI" ]] && needs_link=1
done
if [[ "$needs_link" == 1 ]]; then
	lld-link -out:"$BUILD/BOOTX64.EFI" \
		-subsystem:efi_application \
		-entry:efi_main \
		-nodefaultlib \
		-align:4096 \
		-filealign:4096 \
		"$BUILD"/loader-*.obj
fi

KTARGET=x86_64-ravynos
K_CFLAGS=(--target="$KTARGET"
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mno-red-zone
	-mno-mmx
	-mno-sse
	-mno-sse2
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

clang "${K_CFLAGS[@]}" -c "$ROOT/testkernel/kernel.c" -o "$BUILD/tk-kernel.o"
clang "${K_CFLAGS[@]}" -c "$ROOT/loader/util.c" -o "$BUILD/tk-util.o"
clang --target="$KTARGET" -c "$ROOT/testkernel/start.S" -o "$BUILD/tk-start.o"

kernel_needs_link=0
[[ ! -f "$BUILD/testkernel.elf" ]] && kernel_needs_link=1
for obj in "$BUILD"/tk-*.o; do
	[[ "$obj" -nt "$BUILD/testkernel.elf" ]] && kernel_needs_link=1
done
[[ "$ROOT/testkernel/kernel.ld" -nt "$BUILD/testkernel.elf" ]] && kernel_needs_link=1
if [[ "$kernel_needs_link" == 1 ]]; then
	ld.lld -T "$ROOT/testkernel/kernel.ld" -nostdlib -static \
		-o "$BUILD/testkernel.elf" "$BUILD/tk-start.o" "$BUILD/tk-kernel.o" "$BUILD/tk-util.o"
fi

A64_UEFI_TARGET=aarch64-pc-windows-gnu
A64_UEFI_CFLAGS=(
	--target="$A64_UEFI_TARGET"
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mgeneral-regs-only
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

for src in main elf macho util-arm64; do
	clang "${A64_UEFI_CFLAGS[@]}" -c "$ROOT/loader/$src.c" -o "$BUILD/a64-loader-$src.o"
done
clang --target="$A64_UEFI_TARGET" -c "$ROOT/loader/handoff-arm64.S" -o "$BUILD/a64-loader-handoff.o"

a64_needs_link=0
[[ ! -f "$BUILD/BOOTAA64.EFI" ]] && a64_needs_link=1
for obj in "$BUILD"/a64-loader-*.o; do
	[[ "$obj" -nt "$BUILD/BOOTAA64.EFI" ]] && a64_needs_link=1
done
[[ "$ROOT/loader/aarch64/efi-objlst.ld" -nt "$BUILD/BOOTAA64.EFI" ]] && a64_needs_link=1
if [[ "$a64_needs_link" == 1 ]]; then
	lld-link -out:"$BUILD/BOOTAA64.EFI" \
		-subsystem:efi_application \
		-entry:efi_main \
		-nodefaultlib \
		-align:4096 \
		-filealign:4096 \
		"$BUILD"/a64-loader-*.o
fi

A64_KTARGET=aarch64-ravynos
A64_K_CFLAGS=(--target="$A64_KTARGET"
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mgeneral-regs-only
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

clang "${A64_K_CFLAGS[@]}" -c "$ROOT/testkernel/kernel.c" -o "$BUILD/a64-tk-kernel.o"
clang "${A64_K_CFLAGS[@]}" -c "$ROOT/loader/util-arm64.c" -o "$BUILD/a64-tk-util-arm64.o"
clang --target="$A64_KTARGET" -c "$ROOT/testkernel/start-arm64.S" -o "$BUILD/a64-tk-start.o"

a64_kernel_needs_link=0
[[ ! -f "$BUILD/testkernel-a64.elf" ]] && a64_kernel_needs_link=1
for obj in "$BUILD"/a64-tk-*.o; do
	[[ "$obj" -nt "$BUILD/testkernel-a64.elf" ]] && a64_kernel_needs_link=1
done
[[ "$ROOT/testkernel/kernel-arm64.ld" -nt "$BUILD/testkernel-a64.elf" ]] && a64_kernel_needs_link=1
if [[ "$a64_kernel_needs_link" == 1 ]]; then
	ld.lld -T "$ROOT/testkernel/kernel-arm64.ld" -nostdlib -static \
		-o "$BUILD/testkernel-a64.elf" "$BUILD/a64-tk-start.o" "$BUILD/a64-tk-kernel.o" "$BUILD/a64-tk-util-arm64.o"
fi

printf 'mkfat=%s\nloader=%s\ntestkernel=%s\nloader-aarch64=%s\ntestkernel-aarch64=%s\nloader-riscv64=%s\ntestkernel-riscv64=%s\n' \
	"$BUILD/mkfat" "$BUILD/BOOTX64.EFI" "$BUILD/testkernel.elf" \
	"$BUILD/BOOTAA64.EFI" "$BUILD/testkernel-a64.elf" \
	"$BUILD/BOOTRISCV64.EFI" "$BUILD/testkernel-riscv64.elf"

R64_TARGET=riscv64-efi
R64_UEFI_CFLAGS=(
	--target="$R64_TARGET"
	-fno-pic
	-mcmodel=medany
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mno-relax
	-msmall-data-limit=0
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

for src in main elf util-riscv64; do
	clang "${R64_UEFI_CFLAGS[@]}" -c "$ROOT/loader/$src.c" -o "$BUILD/r64-loader-$src.o"
done
clang "${R64_UEFI_CFLAGS[@]}" -c "$ROOT/loader/macho.c" -o "$BUILD/r64-loader-macho.o"
clang --target="$R64_TARGET" -c "$ROOT/loader/handoff-riscv64.S" -o "$BUILD/r64-loader-handoff.o"

r64_needs_link=0
[[ ! -f "$BUILD/BOOTRISCV64.EFI" ]] && r64_needs_link=1
for obj in "$BUILD"/r64-loader-*.o; do
	[[ "$obj" -nt "$BUILD/BOOTRISCV64.EFI" ]] && r64_needs_link=1
done
if [[ "$r64_needs_link" == 1 ]]; then
	ld.lld -T "$ROOT/loader/riscv64/efi-objlst.ld" -nostdlib -static \
		-e efi_main \
		-o "$BUILD/r64-loader.elf" "$BUILD"/r64-loader-*.o
	riscv64-linux-gnu-objcopy -O pei-riscv64-little "$BUILD/r64-loader.elf" "$BUILD/BOOTRISCV64.EFI.raw"
	"$BUILD/pefix" "$BUILD/BOOTRISCV64.EFI.raw" "$BUILD/BOOTRISCV64.EFI"
	rm -f "$BUILD/BOOTRISCV64.EFI.raw"
fi

R64_KTARGET=riscv64-ravynos
R64_K_CFLAGS=(--target="$R64_KTARGET"
	-fno-pic
	-mcmodel=medany
	-ffreestanding
	-fno-builtin
	-fno-stack-protector
	-fno-stack-check
	-mno-relax
	-msmall-data-limit=0
	-O2
	-Wall
	-Wextra
	-I"$ROOT/include"
	-I"$ROOT/loader"
)

clang "${R64_K_CFLAGS[@]}" -c "$ROOT/testkernel/kernel.c" -o "$BUILD/r64-tk-kernel.o"
clang "${R64_K_CFLAGS[@]}" -c "$ROOT/loader/util-riscv64.c" -o "$BUILD/r64-tk-util-riscv64.o"
clang --target="$R64_KTARGET" -c "$ROOT/testkernel/start-riscv64.S" -o "$BUILD/r64-tk-start.o"

r64_kernel_needs_link=0
[[ ! -f "$BUILD/testkernel-riscv64.elf" ]] && r64_kernel_needs_link=1
for obj in "$BUILD"/r64-tk-*.o; do
	[[ "$obj" -nt "$BUILD/testkernel-riscv64.elf" ]] && r64_kernel_needs_link=1
done
[[ "$ROOT/testkernel/kernel-riscv64.ld" -nt "$BUILD/testkernel-riscv64.elf" ]] && r64_kernel_needs_link=1
if [[ "$r64_kernel_needs_link" == 1 ]]; then
	ld.lld -T "$ROOT/testkernel/kernel-riscv64.ld" -nostdlib -static \
		-o "$BUILD/testkernel-riscv64.elf" "$BUILD/r64-tk-start.o" "$BUILD/r64-tk-kernel.o" "$BUILD/r64-tk-util-riscv64.o"
fi
