#!/bin/sh
set -eu

DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
LLD=${LLD:-ld64.lld}
PYTHON=${PYTHON:-python3}
CC=${CC:-clang}
OUT=guest_kernel

if [ "${1:-}" = "-o" ]; then
	OUT=$2
	shift 2
fi
if [ $# -eq 0 ]; then
	echo "usage: link.sh [-o output] object.o ..." >&2
	exit 2
fi

"$LLD" -arch x86_64 \
	-platform_version macos 10.12.0 11.0.0 \
	-static -no_pie -dead_strip \
	-e __start \
	-map "$OUT.map" \
	"$@" -o "$OUT"

"$PYTHON" "$DIR/patch_macho.py" "$OUT"
"$PYTHON" "$DIR/verify.py" "$OUT"
