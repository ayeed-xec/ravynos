#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Stage-1 toolchain driver: fetch, build and verify the LLVM stage-1 tools with
# a cache keyed by a content hash over every input (the pin, the fetch script,
# the build script and this driver). A change to any input produces a new cache
# directory, so a toolchain is never reused across a pin or build-rule change
# (SYSTEM-DESIGN.md S02 stage-1/D8).
#
# usage: stage1.sh [--verify] [--refetch] [-j <jobs>]
#
#   (no flags)      fetch and build into build/toolchain/<hash>/ unless cached
#   --verify        recompute the hash, re-hash every installed binary and
#                   fail if anything drifted (used by rv bootstrap --verify)
#   --refetch       force a fresh clone of the pinned source
#   -j <jobs>       parallel compile jobs passed to the build

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TREE="$(cd "$SCRIPT_DIR/../.." && pwd)"

VERIFY=0
REFETCH=0
JOBS=""
while [ "$#" -gt 0 ]; do
    case "$1" in
        --verify) VERIFY=1 ;;
        --refetch) REFETCH=1 ;;
        -j)
            shift
            [ "${1:-}" ] && [ "$1" -eq "$1" ] 2>/dev/null || {
                echo "usage: stage1.sh [--verify] [--refetch] [-j <jobs>]" >&2
                exit 2
            }
            JOBS=$1
            ;;
        *) echo "usage: stage1.sh [--verify] [--refetch] [-j <jobs>]" >&2; exit 2 ;;
    esac
    shift
done

# The cache key: content hash over every file that determines what gets built.
HASH_INPUTS=(stage1.sh llvm/_PROVENANCE llvm/fetch.sh llvm/build.sh)
HASH=$(cd "$SCRIPT_DIR" && sha256sum "${HASH_INPUTS[@]}" | sha256sum | awk '{print $1}')
CACHE="$TREE/build/toolchain/$HASH"
STAMP="$CACHE/.stage1-ok"
BIN="$CACHE/bin"
PROVENANCE="$CACHE/provenance"

verify() {
    if [ ! -f "$STAMP" ]; then
        echo "error: stage-1 toolchain $HASH is not built (no $STAMP); run 'rv bootstrap' first" >&2
        return 1
    fi
    if [ "$(head -n 1 "$STAMP")" != "stage1 $HASH" ]; then
        echo "error: $STAMP does not match stage1 $HASH" >&2
        return 1
    fi
    if [ ! -f "$CACHE/tools.sha256" ]; then
        echo "error: $CACHE/tools.sha256 is missing; the cache entry is incomplete, rebuild" >&2
        return 1
    fi
    ( cd "$BIN" && sha256sum -c "$CACHE/tools.sha256" --quiet ) || {
        echo "error: stage-1 binaries in $BIN do not match tools.sha256; rebuild" >&2
        return 1
    }
    for tool in clang ld.lld ld64.lld llvm-ar llvm-nm llvm-symbolizer; do
        "$BIN/$tool" --version >/dev/null || {
            echo "error: $tool does not run; rebuild" >&2
            return 1
        }
    done
    echo "stage1 verify OK: $HASH"
    echo "  clang: $("$BIN/clang" -dumpversion)"
    echo "  dir:   $CACHE"
    return 0
}

if [ "$VERIFY" -eq 1 ]; then
    verify
    exit $?
fi

# Fast path: cache hit.
if [ -f "$STAMP" ] && [ "$(head -n 1 "$STAMP")" = "stage1 $HASH" ]; then
    echo "stage1: cache hit $HASH"
    verify
    exit $?
fi

REFETCH_FLAG=""
if [ "$REFETCH" -eq 1 ]; then
    REFETCH_FLAG="--refetch"
fi

echo "stage1: cache key $HASH"
bash "$SCRIPT_DIR/llvm/fetch.sh" $REFETCH_FLAG

if [ -n "$JOBS" ]; then
    bash "$SCRIPT_DIR/llvm/build.sh" "$TREE/build/vendor/llvm-project" "$CACHE" "$JOBS"
else
    bash "$SCRIPT_DIR/llvm/build.sh" "$TREE/build/vendor/llvm-project" "$CACHE"
fi

{
    echo "stage1 $HASH"
    echo "commit: $(git -C "$TREE" rev-parse HEAD 2>/dev/null || echo none)"
    echo "date:   $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "pin:    $(sed -n 's/^\* \(Tag\|Commit\): /  \1: /p' "$SCRIPT_DIR/llvm/_PROVENANCE")"
} >"$STAMP"

verify
