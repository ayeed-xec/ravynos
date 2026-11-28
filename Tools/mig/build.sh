#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Build mig and migcom from the pristine bootstrap_cmds tree plus the patch
# series. byacc and rvpatch come from the stage-1 build tools (`rv bootstrap`),
# never from the host's package manager.
#
# usage: build.sh [--out <dir>] [--cflag <flag>]... [--smoke]
#
#   --out <dir>     where work/, bin/mig and libexec/migcom go (default: Tools/mig)
#   --cflag <flag>  one extra flag for the migcom compile; repeatable
#   --smoke         run mig over tests/trivial.defs after the build
#
# BYACC and RVPATCH name the tools when they are not on PATH.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
VENDORED="$SCRIPT_DIR/upstream/migcom.tproj"
PATCHES="$SCRIPT_DIR/patches"
SHIM="$SCRIPT_DIR/include"
BYACC=${BYACC:-byacc}
RVPATCH=${RVPATCH:-rvpatch}

usage() {
    echo "usage: $0 [--out <dir>] [--cflag <flag>]... [--smoke]" >&2
    exit 2
}

OUT=$SCRIPT_DIR
SMOKE=0
EXTRA_CFLAGS=()
while [ "$#" -gt 0 ]; do
    case "$1" in
        --out)
            [ "$#" -ge 2 ] || usage
            mkdir -p "$2"
            OUT="$(cd "$2" && pwd)"
            shift
            ;;
        --cflag)
            [ "$#" -ge 2 ] || usage
            EXTRA_CFLAGS+=("$2")
            shift
            ;;
        --smoke) SMOKE=1 ;;
        *) usage ;;
    esac
    shift
done
WORK="$OUT/work"

for tool in "$BYACC" "$RVPATCH" clang; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "error: $tool not found; build the stage-1 tools with 'rv bootstrap'" >&2
        echo "       and put build/toolchain/buildtools-<key>/bin on PATH" >&2
        exit 1
    }
done

TAG=""
while IFS= read -r line; do
    case "$line" in
        "* Tag: "*) TAG="${line#\* Tag: }" ;;
    esac
done <"$SCRIPT_DIR/_PROVENANCE"
if [ -z "$TAG" ]; then
    echo "error: no 'Tag:' line in Tools/mig/_PROVENANCE" >&2
    exit 1
fi

rm -rf "$WORK"
mkdir -p "$WORK" "$OUT/bin" "$OUT/libexec"
cp -R "$VENDORED" "$WORK/migcom.tproj"

# The series file is the order; a patch on disk that the series does not name
# is never applied.
cd "$WORK"
applied=0
while IFS= read -r entry; do
    case "$entry" in
        "" | "#"*) continue ;;
    esac
    [ -f "$PATCHES/$entry" ] || {
        echo "error: patches/series names $entry, which does not exist" >&2
        exit 1
    }
    echo "== applying $entry"
    "$RVPATCH" -p1 "$PATCHES/$entry" || {
        echo "error: $entry does not apply to the pristine tree; fix the series" >&2
        exit 1
    }
    applied=$((applied + 1))
done <"$PATCHES/series"
[ "$applied" -gt 0 ] || {
    echo "error: patches/series names no patch" >&2
    exit 1
}

cd "$WORK/migcom.tproj"
"$BYACC" -d -b y parser.y
cp "$SCRIPT_DIR/generated/lexxer.c" lexxer.c

clang -fuse-ld=lld -O2 -g0 -std=gnu11 -Wall "${EXTRA_CFLAGS[@]}" \
    -I"$SHIM" \
    -DMIG_VERSION="\"$TAG\"" \
    -o "$WORK/migcom" \
    y.tab.c lexxer.c \
    mig.c error.c global.c header.c routine.c server.c \
    statement.c string.c type.c user.c utils.c

install -m 0755 "$WORK/migcom" "$OUT/libexec/migcom"
install -m 0755 "$WORK/migcom.tproj/mig.sh" "$OUT/bin/mig"
echo "== installed: $OUT/bin/mig, $OUT/libexec/migcom"

if [ "$SMOKE" -eq 1 ]; then
    mkdir -p "$WORK/smoke"
    # mig.sh pipes the preprocessor into migcom without pipefail, so a
    # preprocessor error still yields output; its diagnostics are the signal.
    "$OUT/bin/mig" \
        -I"$SHIM" \
        -I"$ROOT/Kernel/xnu/osfmk" \
        -header "$WORK/smoke/trivial.h" \
        -user "$WORK/smoke/trivialUser.c" \
        -server "$WORK/smoke/trivialServer.c" \
        "$SCRIPT_DIR/tests/trivial.defs" 2>"$WORK/smoke/stderr"
    if grep -qE '(^|: )(fatal )?error:' "$WORK/smoke/stderr"; then
        cat "$WORK/smoke/stderr" >&2
        echo "error: mig smoke: the preprocessor or migcom reported an error" >&2
        exit 1
    fi
    for f in trivial.h trivialUser.c trivialServer.c; do
        [ -s "$WORK/smoke/$f" ] || {
            echo "error: mig smoke produced an empty $f" >&2
            exit 1
        }
    done
    echo "== mig smoke OK: trivial.defs -> trivial.h, trivialUser.c, trivialServer.c"
fi
