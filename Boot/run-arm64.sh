#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD="$ROOT/build"

AAVMF_CODE=/usr/share/qemu-efi-aarch64/QEMU_EFI.fd
IMAGE="$BUILD/esp-a64.img"
LOG="$BUILD/serial-a64.log"
MEM=1024
TIMEOUT=90

while [[ $# -gt 0 ]]; do
	case "$1" in
	--image)
		IMAGE="$2"
		shift 2
		;;
	--log)
		LOG="$2"
		shift 2
		;;
	--aavmf)
		AAVMF_CODE="$2"
		shift 2
		;;
	--timeout)
		TIMEOUT="$2"
		shift 2
		;;
	*)
		echo "usage: run-arm64.sh [--image FILE] [--log FILE] [--aavmf FILE] [--timeout SECONDS]" >&2
		exit 2
		;;
	esac
done

command -v qemu-system-aarch64 >/dev/null || {
	echo "run-arm64.sh: qemu-system-aarch64 not installed" >&2
	exit 1
}
[[ -f "$AAVMF_CODE" ]] || {
	echo "run-arm64.sh: AAVMF firmware not found at $AAVMF_CODE" >&2
	exit 1
}
[[ -f "$IMAGE" ]] || {
	echo "run-arm64.sh: disk image not found at $IMAGE" >&2
	exit 1
}

CODE="$BUILD/aavmf-code.fd"
cp "$AAVMF_CODE" "$CODE"
truncate -s 64M "$CODE"
VARS="$BUILD/aavmf-vars.fd"
truncate -s 64M "$VARS"

: >"$LOG"
STATUS=0
timeout --foreground "$TIMEOUT" qemu-system-aarch64 \
	-machine virt \
	-cpu cortex-a57 \
	-m "$MEM" \
	-display none \
	-vga none \
	-monitor none \
	-no-reboot \
	-serial "file:$LOG" \
	-drive "if=pflash,format=raw,readonly=on,file=$CODE" \
	-drive "if=pflash,format=raw,file=$VARS" \
	-drive "if=none,file=$IMAGE,format=raw,id=hd0" \
	-device virtio-blk-pci,drive=hd0 >/dev/null 2>&1 || STATUS=$?

if ! grep -q "RVPANIC BEGIN" "$LOG"; then
	echo "run-arm64.sh: no structured panic block in serial log (qemu status $STATUS)" >&2
	exit 1
fi
if ! grep -q "RVPANIC END" "$LOG"; then
	echo "run-arm64.sh: panic block truncated in serial log (qemu status $STATUS)" >&2
	exit 1
fi
