#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the content hash of every stage-1 tool in a bin directory, as
# "sha256  name" lines in the fixed order of the tool list. Both sides of
# `stage1.sh --verify` call this — the build that records tools.sha256 and the
# rebuild that is compared against it — so the two lists always cover the same
# files in the same form.
#
# usage: hash-tools.sh <bin-dir>   hashes the tools in <bin-dir>
#        hash-tools.sh --list      prints the tool names, one per line

set -euo pipefail

# The stage-1 tool set: what stage 2 and the rv build need, and nothing else
# (SYSTEM-DESIGN.md S02 stage-1/D8). A glob over bin/ would also hash clang-21,
# clang-cl, clang-cpp and llvm-objcopy and would print "./clang" where the
# recorded list says "clang", so a rebuild could never match.
TOOLS=(clang clang++ lld ld.lld ld64.lld lld-link wasm-ld llvm-ar llvm-nm llvm-symbolizer llvm-strip
    llvm-objdump llvm-lipo dsymutil)

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <bin-dir>|--list" >&2
    exit 2
fi

if [ "$1" = "--list" ]; then
    printf '%s\n' "${TOOLS[@]}"
    exit 0
fi

BIN=$1
if [ ! -d "$BIN" ]; then
    echo "error: $BIN is not a directory" >&2
    exit 1
fi

for tool in "${TOOLS[@]}"; do
    if [ ! -f "$BIN/$tool" ]; then
        echo "error: $BIN/$tool is missing" >&2
        exit 1
    fi
done

cd "$BIN"
sha256sum "${TOOLS[@]}"
