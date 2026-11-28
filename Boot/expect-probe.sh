#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Decide whether a test-kernel serial log is a passing boot smoke.
#
# The test kernel always ends in a structured panic, so the presence of a panic
# block says nothing on its own (BUG-26). A boot passes only when the kernel's
# own lines are exactly, in this order: it booted, it validated the boot_args
# hand-off, and it panicked once, on purpose, with reason "probe-reached".
#
# usage: expect-probe.sh <serial-log> <caller-name>
set -euo pipefail

if [[ $# -ne 2 ]]; then
	echo "usage: expect-probe.sh <serial-log> <caller-name>" >&2
	exit 2
fi
LOG=$1
WHO=$2

[[ -f "$LOG" ]] || {
	echo "$WHO: serial log $LOG does not exist" >&2
	exit 1
}

# The kernel writes CRLF, and firmware may leave ANSI escapes on the line it
# shares with the first kernel line. Only whole kernel lines count, and the
# BEGIN line's magic and revision fields are dropped.
esc=$'\033'
events=""
while IFS= read -r raw; do
	line=${raw//$'\r'/}
	while [[ $line == *"${esc}["* ]]; do
		head=${line%%"${esc}["*}
		tail=${line#*"${esc}["}
		while [[ -n $tail && ${tail:0:1} != [A-Za-z] ]]; do
			tail=${tail:1}
		done
		line=${head}${tail:1}
	done
	case "$line" in
	RVTEST\ * | RVPSTR\ * | RVPANIC\ *) ;;
	*) continue ;;
	esac
	case "$line" in
	"RVPANIC BEGIN"*) line="RVPANIC BEGIN" ;;
	"RVPANIC END"*) line="RVPANIC END" ;;
	esac
	events+="${events:+$'\n'}${line}"
done <"$LOG"

expected="RVTEST boot
RVTEST handoff-ok
RVPANIC BEGIN
RVPSTR probe-reached
RVPANIC END"

if [[ "$events" != "$expected" ]]; then
	reasons=""
	while IFS= read -r line; do
		case "$line" in
		"RVPSTR "*) reasons+="${line#RVPSTR } " ;;
		esac
	done <<<"$events"
	echo "$WHO: not a passing boot (panic reason: ${reasons:-none})" >&2
	echo "---- expected kernel lines ----" >&2
	echo "$expected" >&2
	echo "---- kernel lines in $LOG ----" >&2
	echo "${events:-<none>}" >&2
	echo "---- full serial log ----" >&2
	cat "$LOG" >&2
	exit 1
fi

echo "$WHO: hand-off validated, deliberate probe panic captured in $LOG"
