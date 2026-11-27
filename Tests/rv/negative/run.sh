#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Negative fixtures: each one must make `rv graph` fail with the named error code,
# so a gate that silently passes cannot look green (SYSTEM-DESIGN.md 2.7, GAP-33).
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)
RV=${RV:-"$ROOT/Tools/rv/target/debug/rv"}
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

passed=0
failed=0

ok() {
    passed=$((passed + 1))
}

bad() {
    echo "FAIL: $1" >&2
    failed=$((failed + 1))
}

# case <name> <expected code> <component dir> <manifest body>
case_expect_failure() {
    local name=$1 code=$2 dir=$3 body=$4
    local tree="$WORK/$name"
    mkdir -p "$tree/$dir"
    printf '%s\n' "$body" >"$tree/$dir/manifest.toml"
    if (cd "$tree" && "$RV" graph) >"$WORK/out" 2>&1; then
        bad "$name: rv graph accepted an invalid tree"
        return
    fi
    if grep -q -- "$code" "$WORK/out"; then
        ok
    else
        bad "$name: expected $code, got: $(head -1 "$WORK/out")"
    fi
}

# Two components claim the same name; the second must not silently replace the first.
mkdir -p "$WORK/duplicate-name/apps/b"
printf '%s\n' 'name = "same"
layer = "apps"
kind = "executable"
license = "BSD-2-Clause"
state = "experimental"
sources = ["main.c"]' >"$WORK/duplicate-name/apps/b/manifest.toml"

case_expect_failure duplicate-name E-DUP-NAME apps/a 'name = "same"
layer = "apps"
kind = "executable"
license = "BSD-2-Clause"
state = "experimental"
sources = ["main.c"]'

case_expect_failure name-traversal E-SCHEMA apps/a 'name = "../../escape"
layer = "apps"
kind = "executable"
license = "BSD-2-Clause"
state = "experimental"
sources = ["main.c"]'

case_expect_failure unknown-key E-UNKNOWN-KEY apps/a 'name = "a"
layer = "apps"
kind = "executable"
license = "BSD-2-Clause"
state = "experimental"
sources = ["main.c"]
frobnicate = true'

case_expect_failure missing-license E-SCHEMA apps/a 'name = "a"
layer = "apps"
kind = "executable"
state = "experimental"
sources = ["main.c"]'

echo "rv negative tests: $passed passed, $failed failed"
[ "$failed" -eq 0 ]
