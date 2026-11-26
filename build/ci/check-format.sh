#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
status=0

need() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "error: $1 is required for the format gate but was not found." >&2
        echo "       install it (e.g. 'apt install $2' or your distro equivalent) and re-run." >&2
        exit 1
    fi
}

need rustfmt rustfmt
need clang-format clang-format
need git git

echo "== format gate: rustfmt (tools/rv/src)"
tracked() {
    local out
    if ! out=$(git -C "$root" ls-files -- "$@"); then
        echo "error: git ls-files failed in $root" >&2
        exit 1
    fi
    [ -n "$out" ] && printf '%s\n' "$out"
    return 0
}

cd "$root" || exit 1

mapfile -t rs_files < <(tracked ':(glob)tools/rv/src/**/*.rs')
if [ "${#rs_files[@]}" -eq 0 ]; then
    echo "notice: no rust sources under tools/rv/src; nothing to check"
else
    if ! rustfmt --check --edition 2021 "${rs_files[@]}"; then
        echo "error: rustfmt found formatting diffs (shown above); run" >&2
        echo "       'rustfmt --edition 2021 <files>' and commit the result." >&2
        status=1
    fi
fi

echo "== format gate: clang-format (components)"
mapfile -t c_files < <(tracked ':(glob)components/**/*.c' ':(glob)components/**/*.h')
if [ "${#c_files[@]}" -eq 0 ]; then
    echo "notice: no components/** .c/.h files tracked yet; nothing to check"
else
    if ! clang-format --dry-run -Werror "${c_files[@]}" 2> >(sed 's/^/  /' >&2); then
        echo "error: clang-format found violations (shown above); run" >&2
        echo "       'clang-format -i <files>' and commit the result." >&2
        status=1
    fi
fi

echo "== format gate: clang-format (build tooling)"
mapfile -t tool_c_files < <(tracked ':(glob)build/mig/*.c')
if [ "${#tool_c_files[@]}" -eq 0 ]; then
    echo "notice: no build/** .c/.h files tracked yet; nothing to check"
else
    if ! clang-format --dry-run -Werror "${tool_c_files[@]}" 2> >(sed 's/^/  /' >&2); then
        echo "error: clang-format found violations (shown above); run" >&2
        echo "       'clang-format -i <files>' and commit the result." >&2
        status=1
    fi
fi

if [ "$status" -ne 0 ]; then
    echo "format gate: FAILED"
    exit 1
fi
echo "format gate: OK"
