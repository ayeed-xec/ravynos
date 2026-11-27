#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
IMAGE="$ROOT/build/esp.img"
LOG="$ROOT/build/serial.log"
TIMEOUT=90
OVMF_DIR=/usr/share/OVMF
MEM=512

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
	--timeout)
		TIMEOUT="$2"
		shift 2
		;;
	--ovmf-dir)
		OVMF_DIR="$2"
		shift 2
		;;
	*)
		echo "run.sh: unknown option $1" >&2
		exit 2
		;;
	esac
done

[[ -f "$IMAGE" ]]

OVMF_CODE=""
OVMF_VARS=""
for code in "$OVMF_DIR"/OVMF_CODE_4M.fd "$OVMF_DIR"/OVMF_CODE.fd; do
	if [[ -f "$code" ]]; then
		OVMF_CODE="$code"
		OVMF_VARS="${code/OVMF_CODE/OVMF_VARS}"
		break
	fi
done
if [[ -z "$OVMF_CODE" || ! -f "$OVMF_VARS" ]]; then
	echo "run.sh: no OVMF code+vars pair found in $OVMF_DIR" >&2
	exit 1
fi
WORKDIR=$(dirname "$LOG")
mkdir -p "$WORKDIR"
VARS="$WORKDIR/OVMF_VARS.fd"
cp "$OVMF_VARS" "$VARS"

: > "$LOG"
set +e
timeout --foreground "$TIMEOUT" qemu-system-x86_64 \
	-machine q35 \
	-cpu max \
	-m "$MEM" \
	-display none \
	-vga none \
	-serial "file:$LOG" \
	-monitor none \
	-no-reboot \
	-no-shutdown \
	-drive "if=pflash,format=raw,readonly=on,file=$OVMF_CODE" \
	-drive "if=pflash,format=raw,file=$VARS" \
	-drive "format=raw,file=$IMAGE,if=ide"
STATUS=$?
set -e

if [[ $STATUS -ne 0 && $STATUS -ne 124 ]]; then
	echo "run.sh: qemu exited with status $STATUS" >&2
	echo "---- captured serial ----" >&2
	cat "$LOG" >&2
	exit "$STATUS"
fi

if ! grep -q "RVPANIC BEGIN" "$LOG"; then
	echo "run.sh: no structured panic block in serial log (qemu status $STATUS)" >&2
	echo "---- captured serial ----" >&2
	cat "$LOG" >&2
	exit 1
fi
if ! grep -q "RVPANIC END" "$LOG"; then
	echo "run.sh: panic block truncated in serial log" >&2
	cat "$LOG" >&2
	exit 1
fi

echo "run.sh: structured panic block captured in $LOG"
exit 0
