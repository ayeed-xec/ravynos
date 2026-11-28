#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

BOOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../Boot" && pwd)
BUILD="$BOOT/build"

command -v qemu-system-x86_64 >/dev/null 2>&1 || {
	echo "smoke-macho: qemu-system-x86_64 not installed" >&2
	exit 1
}
command -v ld64.lld >/dev/null 2>&1 || {
	echo "smoke-macho: ld64.lld not installed" >&2
	exit 1
}
if [[ ! -d /usr/share/OVMF ]]; then
	echo "smoke-macho: /usr/share/OVMF missing (ovmf package not installed yet)" >&2
	exit 1
fi

bash "$BOOT/build.sh" >/dev/null

M64=(-target x86_64-apple-macosx27.2 -ffreestanding -fno-builtin -fno-pie -mgeneral-regs-only -O2 -Wall -Wextra "-I$BOOT/include" "-I$BOOT/loader")
clang "${M64[@]}" -c "$BOOT/testkernel/kernel.c" -o "$BUILD/tk-macho-kernel.o"
clang "${M64[@]}" -c "$BOOT/loader/util.c" -o "$BUILD/tk-macho-util.o"
clang -target x86_64-apple-macosx27.2 -c "$BOOT/testkernel/start-macho.S" -o "$BUILD/tk-macho-start.o"

ld64.lld -arch x86_64 -e _start -pagezero_size 0x400000 \
	-platform_version macos 27.2 27.2 \
	-o "$BUILD/testkernel-macho" \
	"$BUILD/tk-macho-start.o" "$BUILD/tk-macho-kernel.o" "$BUILD/tk-macho-util.o"

bash "$BOOT/image.sh" \
	--loader "$BUILD/BOOTX64.EFI" \
	--test-kernel "$BUILD/testkernel-macho" \
	--output "$BUILD/esp-macho.img"

bash "$BOOT/run.sh" \
	--image "$BUILD/esp-macho.img" \
	--log "$BUILD/serial-macho.log"

echo "smoke-macho: Mach-O test kernel booted through the loader"
