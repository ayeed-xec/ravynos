#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the cache key of the stage-1 build tools: a hash over every file that
# decides what build.sh produces, the byacc and unifdef pins, and the host
# compiler that builds them. The two keys share no repository input, so a
# change to these tools never moves the LLVM key and an LLVM change never
# rebuilds these; both include the stage-0 compiler version, so upgrading the
# host clang moves both.
#
# usage: key.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

cd "$ROOT"
{
    # Whole directories, so a new patch or header cannot slip past the key.
    find Tools/toolchain/buildtools Tools/toolchain/buildtools.sh \
        Tools/mig/build.sh Tools/mig/_PROVENANCE Tools/mig/rvpatch.c \
        Tools/mig/patches Tools/mig/generated Tools/mig/include Tools/mig/upstream \
        Boot/mkfat/mkfat.c Boot/elf2pe/elf2pe.c -type f -print0 |
        LC_ALL=C sort -z | xargs -0 sha256sum
    bash "$SCRIPT_DIR/vendor-pin.sh" byacc
    bash "$SCRIPT_DIR/vendor-pin.sh" unifdef
    # The first line of `clang --version` is the same on x86_64 and arm64
    # hosts; the target triple and machine tell the binaries apart.
    printf 'host %s %s\n' "$(uname -sm)" "$(clang -dumpmachine 2>/dev/null || echo none)"
    for tool in clang ld.lld; do
        if command -v "$tool" >/dev/null 2>&1; then
            printf '%s %s\n' "$tool" "$("$tool" --version 2>/dev/null | sed -n '1p')"
        else
            printf '%s missing\n' "$tool"
        fi
    done
} | sha256sum | cut -c1-64
