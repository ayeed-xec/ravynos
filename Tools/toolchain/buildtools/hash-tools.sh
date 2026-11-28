#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print "sha256  path" for every installed build tool, in a fixed order, so
# the recorded tools.sha256 and a --verify rebuild list the same files.
#
# usage: hash-tools.sh <install-dir>   hash the tools under <install-dir>
#        hash-tools.sh --list          print the tool paths, one per line

set -euo pipefail

# migcom sits in libexec/ because mig.sh finds it at ../libexec/migcom.
TOOLS=(bin/byacc bin/unifdef bin/rvpatch bin/mkfat bin/elf2pe bin/mig libexec/migcom)

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <install-dir>|--list" >&2
    exit 2
fi
if [ "$1" = "--list" ]; then
    printf '%s\n' "${TOOLS[@]}"
    exit 0
fi

cd "$1"
for tool in "${TOOLS[@]}"; do
    [ -f "$tool" ] || {
        echo "error: $1/$tool is missing" >&2
        exit 1
    }
done
sha256sum "${TOOLS[@]}"
