#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD="$ROOT/build"

RISCV_CODE=/usr/share/qemu-efi-riscv64/RISCV_VIRT_CODE.fd
RISCV_VARS=/usr/share/qemu-efi-riscv64/RISCV_VIRT_VARS.fd
OPENBSI_FW=/usr/lib/riscv64-linux-gnu/opensbi/generic/fw_dynamic.bin
IMAGE="$BUILD/esp-r64.img"
LOG="$BUILD/serial-r64.log"
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
	--opensbi)
		OPENBSI_FW="$2"
		shift 2
		;;
	--timeout)
		TIMEOUT="$2"
		shift 2
		;;
	*)
		echo "usage: run-riscv64.sh [--image FILE] [--log FILE] [--opensbi FILE] [--timeout SECONDS]" >&2
		exit 2
		;;
	esac
done

command -v qemu-system-riscv64 >/dev/null || {
	echo "run-riscv64.sh: qemu-system-riscv64 not installed" >&2
	exit 1
}
[[ -f "$RISCV_CODE" ]] || {
	echo "run-riscv64.sh: edk2 riscv64 firmware not found at $RISCV_CODE" >&2
	exit 1
}
[[ -f "$OPENBSI_FW" ]] || {
	echo "run-riscv64.sh: OpenSBI firmware not found at $OPENBSI_FW (apt-get install opensbi)" >&2
	exit 1
}
[[ -f "$IMAGE" ]] || {
	echo "run-riscv64.sh: disk image not found at $IMAGE" >&2
	exit 1
}

# The firmware writes its variable store; give it a copy, not the system file.
VARS="$BUILD/riscv-vars.fd"
cp "$RISCV_VARS" "$VARS"

: >"$LOG"
STATUS=0
timeout --foreground "$TIMEOUT" qemu-system-riscv64 \
	-machine virt \
	-cpu rv64 \
	-smp 1 \
	-m "$MEM" \
	-display none \
	-vga none \
	-monitor none \
	-no-reboot \
	-serial "file:$LOG" \
	-bios "$OPENBSI_FW" \
	-drive "if=pflash,format=raw,readonly=on,file=$RISCV_CODE" \
	-drive "if=pflash,format=raw,file=$VARS" \
	-drive "if=none,file=$IMAGE,format=raw,id=hd0" \
	-device virtio-blk-pci,drive=hd0 >/dev/null 2>&1 || STATUS=$?

bash "$ROOT/expect-probe.sh" "$LOG" "run-riscv64.sh (qemu status $STATUS)"
