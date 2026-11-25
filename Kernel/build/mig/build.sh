#!/bin/sh
# Build Apple's migcom (bootstrap_cmds-99.231.1, APSL) on Linux.
# Usage: Kernel/build/mig/build.sh [xnu-tree]
# Produces Kernel/build/mig/bin/migcom
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
XNU=${1:-$HERE/../../xnu}
GEN=$HERE/generated
BIN=$HERE/bin
SRC=$HERE/src
SHIM=$HERE/shim

mkdir -p "$GEN" "$BIN"

byacc -d -o "$GEN/y.tab.c" "$SRC/parser.y"
flex -o "$GEN/lexxer.c" "$SRC/lexxer.l"

clang -std=gnu17 -O2 -Ulinux -DMIG_VERSION='"bootstrap_cmds-99.231.1"' \
  -I"$SRC" -I"$SHIM" -I"$GEN" -I"$XNU/osfmk" -I"$XNU/libkern" \
  -include "$SHIM/machine/types.h" \
  -include "$SHIM/mach_decl_shim.h" \
  -Wno-deprecated-non-prototype \
  -o "$BIN/migcom" \
  "$SRC"/error.c "$SRC"/global.c "$SRC"/header.c "$GEN/lexxer.c" "$GEN/y.tab.c" \
  "$SRC"/routine.c "$SRC"/server.c "$SRC"/statement.c "$SRC"/string.c \
  "$SRC"/type.c "$SRC"/user.c "$SRC"/utils.c "$SRC"/mig.c

echo "built $BIN/migcom"
