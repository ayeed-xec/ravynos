#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# The S02 Mach-O assertion: a linked Darwin-target output is a 64-bit Mach-O
# for the expected CPU and file type, and carries LC_BUILD_VERSION with the
# macOS platform (ADR-02) and the deployment version of ADR-01.
#
# usage: check-macho.sh <file> <x86_64|arm64> <EXECUTE|DYLIB|BUNDLE|...>

set -euo pipefail

MINOS=27.2

if [ "$#" -ne 3 ]; then
    echo "usage: $0 <file> <x86_64|arm64> <filetype>" >&2
    exit 2
fi
file=$1 arch=$2 want_type=$3
case "$arch" in
    x86_64) want_cpu=X86_64 ;;
    arm64) want_cpu=ARM64 ;;
    *)
        echo "error: unknown arch $arch" >&2
        exit 2
        ;;
esac

fail() {
    echo "error: $file: $1" >&2
    exit 1
}

[ -f "$file" ] || fail "does not exist"
# llvm-objdump exits 0 on a non-Mach-O input, so judge by what it printed.
headers=$(llvm-objdump --macho --private-headers "$file" 2>&1) || true
grep -q '^Mach header' <<<"$headers" || fail "not a Mach-O file: $headers"

# The header row after "Mach header": magic cputype cpusubtype caps filetype ...
read -r magic cpu _ _ ftype _ < <(grep -A2 '^Mach header' <<<"$headers" | tail -n 1)
[ "$magic" = MH_MAGIC_64 ] || fail "magic is ${magic:-none}, expected MH_MAGIC_64"
[ "$cpu" = "$want_cpu" ] || fail "cputype is ${cpu:-none}, expected $want_cpu"
[ "$ftype" = "$want_type" ] || fail "filetype is ${ftype:-none}, expected MH_$want_type"

build=$(grep -A4 'cmd LC_BUILD_VERSION' <<<"$headers") || fail "no LC_BUILD_VERSION"
grep -qE '^ *platform macos$' <<<"$build" || fail "LC_BUILD_VERSION platform is not macos"
grep -qE "^ *minos $MINOS\$" <<<"$build" || fail "LC_BUILD_VERSION minos is not $MINOS"

echo "Mach-O OK: $file ($want_cpu MH_$want_type, macos $MINOS)"
