#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

BOOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../Boot" && pwd)
BUILD="$BOOT/build"

command -v qemu-system-aarch64 >/dev/null || {
	echo "smoke: qemu-system-aarch64 not installed (apt-get install qemu-system-arm qemu-efi-aarch64)" >&2
	exit 1
}
command -v clang >/dev/null || {
	echo "smoke: clang not installed" >&2
	exit 1
}

bash "$BOOT/build.sh"
bash "$BOOT/image.sh" \
	--loader "$BUILD/BOOTAA64.EFI" \
	--test-kernel "$BUILD/testkernel-a64.elf" \
	--boot-name BOOTAA64.EFI \
	--output "$BUILD/esp-a64.img"
bash "$BOOT/run-arm64.sh" \
	--image "$BUILD/esp-a64.img" \
	--log "$BUILD/serial-a64.log"
bash "$BOOT/symbolicate.sh" \
	--log "$BUILD/serial-a64.log" \
	--elf "$BUILD/testkernel-a64.elf" \
	--out "$BUILD/symbolicated-a64.txt"

echo "==== smoke: symbolicated panic backtrace ===="
cat "$BUILD/symbolicated-a64.txt"
echo "==== smoke: end ===="
