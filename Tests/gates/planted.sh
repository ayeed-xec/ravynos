#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# WP0.5: a gate that cannot fail proves nothing, so each gate is planted with
# the defect it exists to catch, in a throwaway git tree of its own. Every gate
# resolves its root from its own path, which is what lets it run there.
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

passed=0
failed=0
notice=0

ok() {
	passed=$((passed + 1))
	echo "ok: $1"
}

bad() {
	failed=$((failed + 1))
	echo "FAIL: $1" >&2
}

note() {
	notice=$((notice + 1))
	echo "notice: $1"
}

# fresh <name> <gate>: a git tree holding a copy of one gate.
fresh() {
	local d=$work/$1
	mkdir -p "$d/Tools/ci"
	cp "$root/Tools/ci/$2" "$d/Tools/ci/$2"
	git init -q "$d"
	git -C "$d" config user.email tests@ravynos.invalid
	git -C "$d" config user.name tests
	printf '%s\n' "$d"
}

commit_all() {
	git -C "$1" add -A
	git -C "$1" commit -qm fixture
}

# expect_clean <gate> <name>: exit 0, so the plant below is the only difference.
expect_clean() {
	if "$@" >"$work/out" 2>&1; then ok "$2"; else
		bad "$2: clean tree rejected"
		cat "$work/out" >&2
	fi
}

# expect_caught <gate> <name> <file that must be named>: non-zero exit and the
# planted file named in the message.
expect_caught() {
	local gate=$1 name=$2 planted=$3
	if "$gate" >"$work/out" 2>&1; then
		bad "$name: the planted defect was accepted"
	elif ! grep -qF "$planted" "$work/out"; then
		bad "$name: failed but never named $planted"
		cat "$work/out" >&2
	else
		ok "$name"
	fi
}

test_host_tool_gate() {
	local d
	d=$(fresh host check-host-tools.sh)
	mkdir -p "$d/Tools/mig/generated" "$d/Boot"
	cp "$root/Tools/mig/generated/lexxer.c" "$d/Tools/mig/generated/"
	printf '#!/bin/bash\necho building\n' >"$d/Boot/good.sh"
	commit_all "$d"
	expect_clean "$d/Tools/ci/check-host-tools.sh" "host-tool gate passes a clean tree"

	printf '#!/bin/bash\ngcc -o out in.c\n' >"$d/Boot/planted.sh"
	commit_all "$d"
	expect_caught "$d/Tools/ci/check-host-tools.sh" "host-tool gate catches gcc" Boot/planted.sh

	# The gate scans for command position, so the planted file is written with
	# the program name supplied rather than spelled out in this line.
	# shellcheck disable=SC2016 # the planted file needs a literal $(...) body
	printf '#!/bin/bash\nout=$(%s -n 1p in.txt)\n' sed >"$d/Boot/planted.sh"
	commit_all "$d"
	expect_caught "$d/Tools/ci/check-host-tools.sh" "host-tool gate catches sed" Boot/planted.sh
}

test_spdx_gate() {
	local d
	d=$(fresh spdx check-spdx.sh)
	printf '// SPDX-License-Identifier: BSD-2-Clause\nint main(void) { return 0; }\n' >"$d/main.c"
	commit_all "$d"
	expect_clean "$d/Tools/ci/check-spdx.sh" "SPDX gate passes a licensed file"

	printf 'int main(void) { return 0; }\n' >"$d/main.c"
	commit_all "$d"
	expect_caught "$d/Tools/ci/check-spdx.sh" "SPDX gate catches a missing header" main.c
}

test_format_gate() {
	local d pin have
	d=$(fresh format check-format.sh)
	cp "$root/Tools/ci/Containerfile" "$d/Tools/ci/"
	cp "$root/.clang-format" "$d/"
	printf 'int f(int x){return x*2;}\n' >"$d/main.c"
	clang-format -i "$d/main.c"
	commit_all "$d"

	pin=$(grep -oE 'clang-format-[0-9]+' "$root/Tools/ci/Containerfile" | head -n1)
	pin=${pin##*-}
	have=$(clang-format --version)
	have=${have##*version }
	have=${have%%.*}
	if [ "$pin" != "$have" ]; then
		# The gate refuses a formatter the image does not pin, and that refusal
		# is itself worth proving here; the formatting plant runs in the image.
		expect_caught "$d/Tools/ci/check-format.sh" "format gate refuses clang-format $have (image pins $pin)" Containerfile
		note "formatting plant runs in the gate image, where clang-format $pin is installed"
		return 0
	fi

	expect_clean "$d/Tools/ci/check-format.sh" "format gate passes a formatted file"

	printf 'int g(int x) {\n        return x;\n}\n' >"$d/main.c"
	commit_all "$d"
	expect_caught "$d/Tools/ci/check-format.sh" "format gate catches an unformatted file" main.c
}

test_crate_licence_gate() {
	local d
	d=$(fresh crates check-crate-licenses.sh)
	printf 'serde v1.0.229|MIT OR Apache-2.0\nfoo v1.0.0|GPL-2.0-only\n' >"$work/crates.txt"
	if CRATE_LICENSES_FILE="$work/crates.txt" "$d/Tools/ci/check-crate-licenses.sh" >"$work/out" 2>&1; then
		bad "crate licence gate: a GPL crate was accepted"
	elif ! grep -qF 'foo v1.0.0' "$work/out"; then
		bad "crate licence gate: failed but never named the crate"
	else
		ok "crate licence gate catches a GPL crate"
	fi
	printf 'serde v1.0.229|MIT OR Apache-2.0\n' >"$work/crates.txt"
	if CRATE_LICENSES_FILE="$work/crates.txt" "$d/Tools/ci/check-crate-licenses.sh" >/dev/null 2>&1; then
		ok "crate licence gate passes the ADR-24 list"
	else
		bad "crate licence gate: rejected an accepted licence"
	fi
}

test_host_tool_gate
test_spdx_gate
test_format_gate
test_crate_licence_gate

echo "planted gate tests: $passed passed, $failed failed, $notice notice"
[[ $failed -eq 0 ]]
