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

printf 'mkfat=%s\nloader=%s\ntestkernel=%s\n' "$BUILD/mkfat" "$BUILD/BOOTX64.EFI" "$BUILD/testkernel.elf"
