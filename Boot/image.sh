#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD="$ROOT/build"
LOADER="$BUILD/BOOTX64.EFI"
KERNEL=""
TEST_KERNEL=""
OUTPUT=""
SIZE=34
OVMF_DIR=/usr/share/OVMF
AAVMF_DIR=/usr/share/qemu-efi-aarch64
RISCV_EFI_DIR=/usr/share/qemu-efi-riscv64
BOOT_NAME=BOOTX64.EFI
STARTUP=""

while [[ $# -gt 0 ]]; do
	case "$1" in
	--loader)
		LOADER="$2"
		shift 2
		;;
	--kernel)
		KERNEL="$2"
		shift 2
		;;
	--test-kernel)
		TEST_KERNEL="$2"
		shift 2
		;;
	--output)
		OUTPUT="$2"
		shift 2
		;;
	--size)
		SIZE="$2"
		shift 2
		;;
	--ovmf-dir)
		OVMF_DIR="$2"
		shift 2
		;;
	--aavmf-dir)
		AAVMF_DIR="$2"
		shift 2
		;;
	--boot-name)
		BOOT_NAME="$2"
		shift 2
		;;
	--startup)
		STARTUP="$2"
		shift 2
		;;
	*)
		echo "image.sh: unknown option $1" >&2
		exit 2
		;;
	esac
done

if [[ -z "$KERNEL" && -z "$TEST_KERNEL" ]]; then
	echo "image.sh: need --kernel PATH or --test-kernel PATH" >&2
	exit 2
fi
if [[ -z "$OUTPUT" ]]; then
	OUTPUT="$BUILD/esp.img"
fi
mkdir -p "$(dirname "$OUTPUT")"

MKFAT="$BUILD/mkfat"
if [[ ! -x "$MKFAT" ]]; then
	mkdir -p "$BUILD"
	clang -O2 -Wall -o "$MKFAT" "$ROOT/mkfat/mkfat.c"
fi

KERNEL_FILE="$KERNEL"
if [[ -z "$KERNEL_FILE" ]]; then
	KERNEL_FILE="$TEST_KERNEL"
fi
[[ -f "$KERNEL_FILE" ]]

ESP_FILES=("EFI/BOOT/$BOOT_NAME" "$LOADER" "KERNEL.ELF" "$KERNEL_FILE")
if [[ -n "$STARTUP" ]]; then
	STARTUP_SCRIPT="$BUILD/startup.nsh"
	printf '%s\r\n' "$STARTUP" >"$STARTUP_SCRIPT"
	ESP_FILES+=("STARTUP.NSH" "$STARTUP_SCRIPT")
fi
"$MKFAT" "$OUTPUT" "$SIZE" "${ESP_FILES[@]}"

FIRMWARE=""
case "$BOOT_NAME" in
BOOTX64.EFI)
	for candidate in "$OVMF_DIR"/OVMF_CODE_4M.fd "$OVMF_DIR"/OVMF_CODE.fd; do
		if [[ -f "$candidate" ]]; then
			FIRMWARE="$candidate"
			break
		fi
	done
	[[ -n "$FIRMWARE" ]] || {
		echo "image.sh: no OVMF firmware found in $OVMF_DIR" >&2
		exit 1
	}
	;;
BOOTAA64.EFI)
	FIRMWARE="$AAVMF_DIR/QEMU_EFI.fd"
	[[ -f "$FIRMWARE" ]] || {
		echo "image.sh: no AAVMF firmware found at $FIRMWARE" >&2
		exit 1
	}
	;;
BOOTRISCV64.EFI)
	FIRMWARE="$RISCV_EFI_DIR/RISCV_VIRT_CODE.fd"
	[[ -f "$FIRMWARE" ]] || {
		echo "image.sh: no RISC-V edk2 firmware found at $FIRMWARE" >&2
		exit 1
	}
	;;
*)
	echo "image.sh: unsupported boot name $BOOT_NAME" >&2
	exit 2
	;;
esac

printf 'esp_image=%s\nfirmware=%s\nkernel=%s\n' "$OUTPUT" "$FIRMWARE" "$KERNEL_FILE"
