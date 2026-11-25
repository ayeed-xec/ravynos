#!/bin/sh
set -eu

DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CC=${CC:-clang}

"$CC" -target x86_64-apple-macos10.12 \
	-ffreestanding -fno-pie -mno-red-zone -Os \
	-fstack-protector-strong \
	-c "$DIR/main.c" -o "$DIR/main.o"

exec sh "$DIR/../link.sh" -o "$DIR/../${1:-guest_kernel}" "$DIR/main.o"
