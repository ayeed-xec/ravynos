#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Stage-1 build tools: byacc, unifdef, rvpatch, mkfat, elf2pe, mig and migcom,
# cached in build/toolchain/buildtools-<key>/ under their own key
# (buildtools/key.sh). This is a separate component from LLVM (stage1.sh):
# the two keys share no input, so either can change without rebuilding the
# other.
#
# usage: buildtools.sh [--verify] [--refetch] [--print-bin]
#
#   (no flags)    fetch and build unless the cache entry exists
#   --verify      rebuild into a scratch directory and compare its hashes with
#                 the recorded tools.sha256, then re-hash the installed tools
#   --refetch     fetch the pinned byacc and unifdef trees again
#   --print-bin   print the bin directory for the current key and exit

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
HERE="$SCRIPT_DIR/buildtools"

VERIFY=0
REFETCH=""
PRINT_BIN=0
for arg in "$@"; do
    case "$arg" in
        --verify) VERIFY=1 ;;
        --refetch) REFETCH=--refetch ;;
        --print-bin) PRINT_BIN=1 ;;
        *)
            echo "usage: buildtools.sh [--verify] [--refetch] [--print-bin]" >&2
            exit 2
            ;;
    esac
done

KEY=$(bash "$HERE/key.sh")
CACHE="$ROOT/build/toolchain/buildtools-$KEY"
STAMP="$CACHE/.buildtools-ok"

if [ "$PRINT_BIN" -eq 1 ]; then
    echo "$CACHE/bin"
    exit 0
fi

built() {
    [ -f "$STAMP" ] && [ "$(head -n 1 "$STAMP")" = "buildtools $KEY" ] &&
        [ -f "$CACHE/tools.sha256" ]
}

check_installed() {
    (cd "$CACHE" && sha256sum --quiet -c tools.sha256) || {
        echo "error: installed build tools in $CACHE do not match tools.sha256; rebuild" >&2
        return 1
    }
    "$CACHE/bin/byacc" -V >/dev/null 2>&1
    "$CACHE/bin/unifdef" -V >/dev/null 2>&1
    echo "buildtools OK: $KEY"
    echo "  bin: $CACHE/bin"
}

fetch_sources() {
    bash "$HERE/fetch.sh" byacc $REFETCH
    bash "$HERE/fetch.sh" unifdef $REFETCH
}

if [ "$VERIFY" -eq 1 ]; then
    built || {
        echo "error: build tools $KEY are not built; run 'rv bootstrap' first" >&2
        exit 1
    }
    fetch_sources
    scratch="$CACHE.verify.$$"
    trap 'rm -rf "$scratch"' EXIT
    bash "$HERE/build.sh" "$scratch"
    if ! diff -u "$CACHE/tools.sha256" "$scratch/tools.sha256"; then
        echo "error: verify: the rebuilt tools differ from the recorded tools.sha256" >&2
        exit 1
    fi
    echo "buildtools verify: rebuild matches the recorded hashes"
    check_installed
    exit
fi

if built; then
    echo "buildtools: cache hit $KEY"
    check_installed
    exit
fi

echo "buildtools: cache key $KEY"
fetch_sources
rm -rf "$CACHE"
bash "$HERE/build.sh" "$CACHE"
{
    echo "buildtools $KEY"
    echo "commit: $(git -C "$ROOT" rev-parse HEAD 2>/dev/null || echo none)"
    for name in byacc unifdef; do
        mapfile -t pin < <(bash "$HERE/vendor-pin.sh" "$name")
        echo "$name: ${pin[1]} ${pin[2]}"
    done
} >"$STAMP"
check_installed
