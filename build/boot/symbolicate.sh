#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
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

llvm-nm -n "$ELF" | awk '$2 ~ /[tTdrRbB]/ { print $1, $3 }' | sort > "$ROOT/build/symbols.sorted"

symbolicate() {
	local addr=$1
	local dec=$(( 16#${addr#0x} ))
	local line
	line=$(awk -v a="$dec" 'function h(x){v=0; for(i=1;i<=length(x);i++){c=tolower(substr(x,i,1)); p=index("0123456789abcdef",c); if(p==0) continue; v=v*16+p-1} return v} {v=h($1); if(v<=a){s=v; n=$2}} END {print n, s}' "$ROOT/build/symbols.sorted")
	local sym
	sym=${line%% *}
	local base
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
