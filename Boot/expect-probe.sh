#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Decide whether a test-kernel serial log is a passing boot smoke.
#
# The test kernel always ends in a structured panic, so the presence of a panic
# block says nothing on its own (BUG-26). A boot passes only when the kernel
# validated the boot_args hand-off ("RVTEST handoff-ok") and then panicked on
# purpose with reason "probe-reached". Any other reason is a real failure.
#
# usage: expect-probe.sh <serial-log> <caller-name>
set -euo pipefail

if [[ $# -ne 2 ]]; then
	echo "usage: expect-probe.sh <serial-log> <caller-name>" >&2
	exit 2
fi
LOG=$1
WHO=$2

fail() {
	echo "$WHO: $1" >&2
	echo "---- captured serial ----" >&2
	cat "$LOG" >&2
	exit 1
}

[[ -f "$LOG" ]] || fail "serial log $LOG does not exist"
grep -q "RVPANIC BEGIN" "$LOG" || fail "no structured panic block in serial log"
grep -q "RVPANIC END" "$LOG" || fail "panic block truncated in serial log"
grep -q "RVTEST handoff-ok" "$LOG" || fail "kernel did not validate the boot_args hand-off"

reasons=$(sed -n 's/^RVPSTR //p' "$LOG" | tr -d '\r')
[[ "$reasons" == "probe-reached" ]] ||
	fail "kernel panicked with '${reasons:-<no reason>}', expected exactly 'probe-reached'"

echo "$WHO: hand-off validated, deliberate probe panic captured in $LOG"
