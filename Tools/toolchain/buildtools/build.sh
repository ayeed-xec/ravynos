#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Build the stage-1 build tools that are not LLVM: byacc and unifdef from their
# fetched pins, rvpatch, mkfat and elf2pe from our sources, and mig/migcom from
# Tools/mig with the byacc and rvpatch built here (SYSTEM-DESIGN.md S02).
# Stage 0 (the host clang and lld) compiles them; nothing runs configure, make,
# bison or flex.
#
# usage: build.sh <install-dir>
#
# The fetched trees must already be in build/vendor/{byacc,unifdef}
# (fetch.sh). <install-dir> receives bin/, libexec/ and tools.sha256.

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <install-dir>" >&2
    exit 2
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
mkdir -p "$1"
OUT="$(cd "$1" && pwd -P)"
BYACC_SRC="$ROOT/build/vendor/byacc"
UNIFDEF_SRC="$ROOT/build/vendor/unifdef"

for tool in clang ld.lld; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "error: $tool (stage 0) is required to build the stage-1 build tools" >&2
        exit 1
    }
done
for src in "$BYACC_SRC/main.c" "$UNIFDEF_SRC/unifdef.c"; do
    [ -f "$src" ] || {
        echo "error: $src is missing; fetch the tree first (fetch.sh)" >&2
        exit 1
    }
done

rm -rf "${OUT:?}/bin" "${OUT:?}/libexec" "${OUT:?}/obj"
mkdir -p "$OUT/bin" "$OUT/libexec" "$OUT/obj/unifdef"

# The prefix maps keep the checkout and install paths out of the binaries, so
# a rebuild elsewhere (--verify) produces the same bytes.
CC=(clang -O2 -g0 -fuse-ld=lld "-ffile-prefix-map=$ROOT=/src" "-ffile-prefix-map=$OUT=/out")

echo "buildtools: byacc"
byacc_srcs=(closure.c error.c graph.c lalr.c lr0.c main.c mkpar.c mstring.c
    output.c reader.c yaccpar.c symtab.c verbose.c warshall.c)
(cd "$BYACC_SRC" && "${CC[@]}" -DHAVE_CONFIG_H -I"$SCRIPT_DIR/byacc" -I. \
    -o "$OUT/bin/byacc" "${byacc_srcs[@]}")

echo "buildtools: unifdef"
# unifdef's version.h comes from a git-describe script upstream; the pin is the
# version, so write it from VENDORS.
mapfile -t pin < <(bash "$SCRIPT_DIR/vendor-pin.sh" unifdef)
# shellcheck disable=SC2016 # $Version$ is an RCS keyword, not a variable
printf '"@(#) $Version: %s $\\n"\n' "${pin[1]}" >"$OUT/obj/unifdef/version.h"
"${CC[@]}" -I"$OUT/obj/unifdef" -I"$UNIFDEF_SRC" -o "$OUT/bin/unifdef" "$UNIFDEF_SRC/unifdef.c"

echo "buildtools: rvpatch, mkfat, elf2pe"
OURS=(-std=gnu11 -Wall -Wextra -Werror)
"${CC[@]}" "${OURS[@]}" -o "$OUT/bin/rvpatch" "$ROOT/Tools/mig/rvpatch.c"
"${CC[@]}" "${OURS[@]}" -o "$OUT/bin/mkfat" "$ROOT/Boot/mkfat/mkfat.c"
"${CC[@]}" "${OURS[@]}" -o "$OUT/bin/elf2pe" "$ROOT/Boot/elf2pe/elf2pe.c"

echo "buildtools: mig"
BYACC="$OUT/bin/byacc" RVPATCH="$OUT/bin/rvpatch" \
    bash "$ROOT/Tools/mig/build.sh" --out "$OUT/obj/mig" \
    --cflag "-ffile-prefix-map=$ROOT=/src" --cflag "-ffile-prefix-map=$OUT=/out" \
    >"$OUT/obj/mig.log" 2>&1 || {
    cat "$OUT/obj/mig.log" >&2
    exit 1
}
install -m 0755 "$OUT/obj/mig/bin/mig" "$OUT/bin/mig"
install -m 0755 "$OUT/obj/mig/libexec/migcom" "$OUT/libexec/migcom"

bash "$SCRIPT_DIR/hash-tools.sh" "$OUT" >"$OUT/tools.sha256"
rm -rf "$OUT/obj"
echo "buildtools: built $(bash "$SCRIPT_DIR/hash-tools.sh" --list | tr '\n' ' ')"
