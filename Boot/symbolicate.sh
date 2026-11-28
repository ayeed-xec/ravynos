#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
LOG="$ROOT/build/serial.log"
ELF=""
OUT=""

while [[ $# -gt 0 ]]; do
	case "$1" in
	--log)
		LOG="$2"
		shift 2
		;;
	--elf)
		ELF="$2"
		shift 2
		;;
	--out)
		OUT="$2"
		shift 2
		;;
	*)
		echo "symbolicate.sh: unknown option $1" >&2
		exit 2
		;;
	esac
done

[[ -f "$LOG" ]]
if [[ -z "$ELF" ]]; then
	for candidate in "$ROOT/build/testkernel.elf" "$ROOT/build/kernel.elf"; do
		if [[ -f "$candidate" ]]; then
			ELF="$candidate"
			break
		fi
	done
fi
[[ -n "$ELF" && -f "$ELF" ]]

SYMBOLIZER=""
if command -v llvm-symbolizer >/dev/null 2>&1; then
	SYMBOLIZER=$(command -v llvm-symbolizer)
fi

llvm-nm -n "$ELF" | while read -r addr type name; do
	case "$type" in
	[tTdrRbB]) ;;
	*) continue ;;
	esac
	[[ -n $name && $name != .L* ]] || continue
	printf '%s %s\n' "$addr" "$name"
done | sort -u >"$ROOT/build/symbols.sorted"

symbolicate() {
	local addr=$1
	local dec=$((16#${addr#0x}))
	local line=""
	local symaddr sym
	while read -r symaddr sym; do
		local a=$((16#${symaddr#0x}))
		[[ $a -le $dec ]] || break
		line="$sym $a"
	done <"$ROOT/build/symbols.sorted"
	local base
	sym=${line%% *}
	base=${line##* }
	if [[ -z "$sym" ]]; then
		echo "?+$addr"
		return
	fi
	local off=$(( dec - base ))
	if [[ $off -eq 0 ]]; then
		echo "$sym"
	else
		echo "$sym+0x$(printf %x "$off")"
	fi
}

SYMBOLICATED="$ROOT/build/symbolicated.txt"
: > "$SYMBOLICATED"

while IFS= read -r line; do
	line=${line%$'\r'}
	case "$line" in
	RVPSTR\ *)
		echo "reason: ${line#RVPSTR }" | tee -a "$SYMBOLICATED"
		;;
	RVFRAME\ *)
		addr=${line#RVFRAME }
		frame=$(symbolicate "$addr")
		if [[ -n "$SYMBOLIZER" ]]; then
			loc=$("$SYMBOLIZER" --obj="$ELF" "$addr" | head -1)
			echo "frame: $addr $frame ($loc)" | tee -a "$SYMBOLICATED"
		else
			echo "frame: $addr $frame" | tee -a "$SYMBOLICATED"
		fi
		;;
	esac
done < "$LOG"

if [[ -n "$OUT" && ! "$OUT" -ef "$SYMBOLICATED" ]]; then
	cp "$SYMBOLICATED" "$OUT"
fi
exit 0
