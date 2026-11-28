#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Every C, header and Rust source we own is formatted by the tools the gate
# image pins (SYSTEM-DESIGN.md 2.2, 5.6, GAP-56). Vendored trees keep upstream
# formatting and are out of scope.
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
status=0

need() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "error: $1 is required for the format gate but was not found." >&2
        exit 1
    fi
}

need rustfmt rustfmt
need clang-format clang-format
need git git

cd "$root" || exit 1

# The image recipe is the single source of the formatter's version: a gate run
# with another clang-format would disagree with CI about the same file.
pin=$(grep -oE 'clang-format-[0-9]+' Tools/ci/Containerfile | head -n1)
pin=${pin##*-}
have=$(clang-format --version)
have=${have##*version }
have=${have%%.*}
if [ -z "$pin" ] || [ "$pin" != "$have" ]; then
    echo "error: clang-format major $have, but Tools/ci/Containerfile pins $pin." >&2
    echo "       run the gate in the image: docker build -f Tools/ci/Containerfile \\" >&2
    echo "       -t ravynos-gates Tools/ci && docker run --rm -v \"\$PWD:/src\" -w /src \\" >&2
    echo "       ravynos-gates Tools/ci/check-format.sh" >&2
    exit 1
fi

# collect <pathspecs>: one tracked path per line; a git failure is reported and
# returned, so the caller can stop instead of checking an empty list.
collect() {
    local out
    if ! out=$(git -C "$root" ls-files -- "$@"); then
        echo "error: git ls-files failed in $root" >&2
        return 1
    fi
    [ -n "$out" ] && printf '%s\n' "$out"
    return 0
}

owned_c() {
    local file
    while IFS= read -r file; do
        case "$file" in
        Kernel/xnu/* | Tools/mig/upstream/* | Tools/mig/tests/* | build/*) continue ;;
        esac
        printf '%s\n' "$file"
    done
}

echo "== format gate: rustfmt"
if ! rs_list=$(collect ':(glob)Tools/rv/src/**/*.rs' ':(glob)Tools/xnuhook/src/**/*.rs' ':(glob)Tools/*/tests/**/*.rs'); then
    echo "format gate: FAILED"
    exit 1
fi
rs_files=()
[ -n "$rs_list" ] && mapfile -t rs_files <<<"$rs_list"
if [ "${#rs_files[@]}" -eq 0 ]; then
    echo "notice: no Rust sources tracked yet; nothing to check"
elif ! rustfmt --check --edition 2021 "${rs_files[@]}"; then
    echo "error: rustfmt found formatting diffs (shown above); run" >&2
    echo "       'rustfmt --edition 2021 <files>' and commit the result." >&2
    status=1
fi

echo "== format gate: clang-format"
if ! c_list=$(collect '*.c' '*.h' '*.m' '*.mm'); then
    echo "format gate: FAILED"
    exit 1
fi
c_files=()
while IFS= read -r file; do
    [ -n "$file" ] || continue
    c_files+=("$file")
done < <(printf '%s\n' "$c_list" | owned_c)
if [ "${#c_files[@]}" -eq 0 ]; then
    echo "notice: no C sources tracked yet; nothing to check"
else
    out=$(clang-format --dry-run -Werror "${c_files[@]}" 2>&1) || {
        printf '%s\n' "$out" >&2
        echo "error: clang-format found violations (shown above); run" >&2
        echo "       'clang-format -i <files>' in the gate image and commit the result." >&2
        status=1
    }
fi

if [ "$status" -ne 0 ]; then
    echo "format gate: FAILED"
    exit 1
fi
echo "format gate: OK"
