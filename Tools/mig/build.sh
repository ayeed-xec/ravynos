#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RAVYNOS_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
VENDORED="$RAVYNOS_ROOT/Tools/mig/upstream/migcom.tproj"
PATCHES="$SCRIPT_DIR/patches"
SHIM="$SCRIPT_DIR/include"
WORK="$SCRIPT_DIR/work"
BIN="$SCRIPT_DIR/bin"
LIBEXEC="$SCRIPT_DIR/libexec"

TAG=""
while IFS= read -r line; do
    case "$line" in
        "* Tag: "*) TAG="${line#\* Tag: }" ;;
    esac
done < "$RAVYNOS_ROOT/Tools/mig/_PROVENANCE"
if [ -z "$TAG" ]; then
    echo "error: no 'Tag:' line in Tools/mig/_PROVENANCE" >&2
    exit 1
fi

need_pkg=()
command -v byacc >/dev/null 2>&1 || need_pkg+=(byacc)
command -v clang >/dev/null 2>&1 || need_pkg+=(clang)
if [ ${#need_pkg[@]} -gt 0 ]; then
    echo "installing missing toolchain packages: ${need_pkg[*]}"
    sudo apt-get update -qq && sudo apt-get install -y -qq "${need_pkg[@]}"
fi
for tool in byacc clang; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "error: $tool still missing after apt-get install" >&2; exit 1; }
done

if [ "${1:-}" = "--regen-lexer" ]; then
    echo "error: the lexer is hand-written; there is nothing to regenerate" >&2
    exit 1
fi

rm -rf "$WORK"
mkdir -p "$WORK" "$BIN"
clang -fuse-ld=lld -O2 -std=gnu11 -Wall -Wextra -Werror -o "$BIN/rvpatch" "$SCRIPT_DIR/rvpatch.c"
cp -R "$VENDORED" "$WORK/migcom.tproj"

cd "$WORK"
for pf in "$PATCHES"/*.patch; do
    [ -e "$pf" ] || { echo "error: no patches found in $PATCHES" >&2; exit 1; }
    echo "== applying ${pf##*/}"
    "$BIN/rvpatch" -p1 "$pf" || {
        echo "error: hunk application FAILED for ${pf##*/}" >&2
        echo "       (the series must apply cleanly onto pristine" >&2
        echo "        sources; fix the series, not the vendored files)" >&2
        exit 1
    }
done

cd "$WORK/migcom.tproj"
byacc -d -b y parser.y
cp "$SCRIPT_DIR/generated/lexxer.c" lexxer.c

clang -fuse-ld=lld -O2 -std=gnu11 -Wall \
    -I"$SHIM" \
    -DMIG_VERSION="\"$TAG\"" \
    -o "$WORK/migcom" \
    y.tab.c lexxer.c \
    mig.c error.c global.c header.c routine.c server.c \
    statement.c string.c type.c user.c utils.c

mkdir -p "$BIN" "$LIBEXEC"
install -m 0755 "$WORK/migcom" "$LIBEXEC/migcom"
install -m 0755 "$WORK/migcom.tproj/mig.sh" "$BIN/mig"

echo "== installed: $BIN/mig, $LIBEXEC/migcom"

if [ "${1:-}" = "--smoke" ]; then
    SMOKE="$SCRIPT_DIR/tests"
    OUT="$WORK/smoke"
    mkdir -p "$OUT"
    "$BIN/mig" \
        -I"$SHIM" \
        -I"$RAVYNOS_ROOT/Kernel/xnu/osfmk" \
        -header "$OUT/trivial.h" \
        -user "$OUT/trivialUser.c" \
        -server "$OUT/trivialServer.c" \
        "$SMOKE/trivial.defs"
    if [ ! -s "$OUT/trivial.h" ]; then
        echo "error: smoke test produced an empty header" >&2
        exit 1
    fi
    echo "== smoke test OK:"
    head -20 "$OUT/trivial.h"
fi
