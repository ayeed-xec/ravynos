#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

BOOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../Boot" && pwd)
BUILD="$BOOT/build"

command -v qemu-system-riscv64 >/dev/null || {
	echo "smoke: qemu-system-riscv64 not installed (apt-get install qemu-system-misc qemu-efi-riscv64 opensbi)" >&2
	exit 1
}
command -v clang >/dev/null || {
	echo "smoke: clang not installed" >&2
	exit 1
}

bash "$BOOT/build.sh"
bash "$BOOT/image.sh" \
	--loader "$BUILD/BOOTRISCV64.EFI" \
	--test-kernel "$BUILD/testkernel-riscv64.elf" \
	--boot-name BOOTRISCV64.EFI \
	--startup '\EFI\BOOT\BOOTRISCV64.EFI' \
	--output "$BUILD/esp-r64.img"
bash "$BOOT/run-riscv64.sh" \
	--image "$BUILD/esp-r64.img" \
	--log "$BUILD/serial-r64.log"
bash "$BOOT/symbolicate.sh" \
	--log "$BUILD/serial-r64.log" \
	--elf "$BUILD/testkernel-riscv64.elf" \
	--out "$BUILD/symbolicated-r64.txt"

echo "==== smoke: symbolicated panic backtrace ===="
cat "$BUILD/symbolicated-r64.txt"
echo "==== smoke: end ===="
