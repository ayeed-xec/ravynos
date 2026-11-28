#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the stage-1 cache key: a hash over every input that decides what gets
# built, plus the versions of the host tools that do the building. A key that
# ignored the host compiler would hand back binaries built by a different one.
#
# usage: key.sh [<stage1-dir>]   (default: the directory above this script)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STAGE1_DIR=${1:-$(cd "$SCRIPT_DIR/.." && pwd)}

# Relative to the stage-1 directory. key.sh is listed because a change to how
# the key is computed changes what the key means.
inputs=(stage1.sh llvm/_PROVENANCE llvm/fetch.sh llvm/build.sh llvm/hash-tools.sh llvm/key.sh)
host_tools=(cmake ninja clang clang++)

host_version() {
    if command -v "$1" >/dev/null 2>&1; then
        # sed, not head: head exits early and pipefail turns the SIGPIPE into a
        # failed key computation.
        "$1" --version 2>/dev/null | sed -n '1p'
    else
        printf 'missing\n'
    fi
}

{
    (cd "$STAGE1_DIR" && sha256sum "${inputs[@]}")
    for tool in "${host_tools[@]}"; do
        printf '%s %s\n' "$tool" "$(host_version "$tool")"
    done
} | sha256sum | awk '{print $1}'
