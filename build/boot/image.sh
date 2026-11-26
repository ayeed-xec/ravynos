#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
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

"$MKFAT" "$OUTPUT" "$SIZE" "EFI/BOOT/BOOTX64.EFI" "$LOADER" "KERNEL.ELF" "$KERNEL_FILE"

OVMF_CODE=""
for candidate in "$OVMF_DIR"/OVMF_CODE_4M.fd "$OVMF_DIR"/OVMF_CODE.fd; do
	if [[ -f "$candidate" ]]; then
		OVMF_CODE="$candidate"
		break
	fi
done
if [[ -z "$OVMF_CODE" ]]; then
	echo "image.sh: no OVMF firmware found in $OVMF_DIR" >&2
	exit 1
fi

printf 'esp_image=%s\novmf_code=%s\nkernel=%s\n' "$OUTPUT" "$OVMF_CODE" "$KERNEL_FILE"
