#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the cache key of the stage-1 build tools: a hash over every file that
# decides what build.sh produces, the byacc and unifdef pins, and the host
# compiler that builds them. Nothing here is an LLVM input and llvm/key.sh
# reads nothing here, so a change to these tools never invalidates the LLVM
# cache entry, and an LLVM change never rebuilds these.
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
    for tool in clang ld.lld; do
        if command -v "$tool" >/dev/null 2>&1; then
            printf '%s %s\n' "$tool" "$("$tool" --version 2>/dev/null | sed -n '1p')"
        else
            printf '%s missing\n' "$tool"
        fi
    done
} | sha256sum | cut -c1-64
