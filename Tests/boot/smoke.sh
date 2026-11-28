#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

BOOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../Boot" && pwd)
BUILD="$BOOT/build"

command -v qemu-system-x86_64 >/dev/null 2>&1 || {
	echo "smoke: qemu-system-x86_64 not installed" >&2
	exit 1
}
command -v clang >/dev/null 2>&1 || {
	echo "smoke: clang not installed" >&2
	exit 1
}
if [[ ! -d /usr/share/OVMF ]]; then
	echo "smoke: /usr/share/OVMF missing (ovmf package not installed yet)" >&2
	exit 1
fi

bash "$BOOT/build.sh"

bash "$BOOT/image.sh" \
	--loader "$BUILD/BOOTX64.EFI" \
	--test-kernel "$BUILD/testkernel.elf" \
	--output "$BUILD/esp.img"

bash "$BOOT/run.sh" \
	--image "$BUILD/esp.img" \
	--log "$BUILD/serial.log"

bash "$BOOT/symbolicate.sh" \
	--log "$BUILD/serial.log" \
	--elf "$BUILD/testkernel.elf" \
	--out "$BUILD/symbolicated.txt"

echo "==== smoke: symbolicated panic backtrace ===="
cat "$BUILD/symbolicated.txt"
echo "==== smoke: end ===="
