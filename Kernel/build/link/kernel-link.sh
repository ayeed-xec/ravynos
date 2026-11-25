#!/bin/sh
# kernel-link.sh - link a set of Mach-O objects into a static guest kernel
# and run the same post-link patch + verify chain as link.sh.
#
# Usage: kernel-link.sh [-o output] [-e entry_symbol] [-l listfile] object.o ...
#
#   -o output        output Mach-O path (default: <script dir>/guest_kernel)
#   -e entry_symbol  entry symbol passed to ld64.lld -e (default: __start).
#                    Set this when linking real kernel objects without the
#                    stub main.o (which owns __start).
#   -l listfile      read additional object paths from listfile, one per
#                    line; blank lines and lines starting with '#' are
#                    skipped. Paths are relative to the CWD when the script
#                    is run.
#
# Extra ld64.lld flags (e.g. -U _symbol to tolerate not-yet-provided
# undefined symbols) can be passed via the EXTRA_LD_FLAGS environment
# variable; it is word-split like a normal shell expansion.
#
# Objects are given as arguments and/or via -l; at least one is required.
set -eu

EXTRA_LD_FLAGS=${EXTRA_LD_FLAGS:-}
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
LLD=${LLD:-ld64.lld}
PYTHON=${PYTHON:-python3}
ENTRY=__start
OUT=$DIR/guest_kernel
LIST=

while getopts "o:e:l:" opt; do
	case "$opt" in
	o) OUT=$OPTARG ;;
	e) ENTRY=$OPTARG ;;
	l) LIST=$OPTARG ;;
	*) echo "usage: kernel-link.sh [-o output] [-e entry_symbol] [-l listfile] object.o ..." >&2
	   exit 2 ;;
	esac
done
shift $((OPTIND - 1))

if [ $# -eq 0 ] && [ -z "$LIST" ]; then
	echo "usage: kernel-link.sh [-o output] [-e entry_symbol] [-l listfile] object.o ..." >&2
	exit 2
fi

OBJS=""
for obj in "$@"; do
	OBJS="$OBJS $obj"
done
if [ -n "$LIST" ]; then
	while IFS= read -r line || [ -n "$line" ]; do
		case "$line" in
			''|'#'*) continue ;;
		esac
		OBJS="$OBJS $line"
	done < "$LIST"
fi

# Same link chain as link.sh: static Mach-O, no PIE, dead stripped,
# entry chosen by symbol name, map file next to the output.
# shellcheck disable=SC2086
"$LLD" -arch x86_64 \
	-platform_version macos 10.12.0 11.0.0 \
	-static -no_pie -dead_strip \
	-e "$ENTRY" \
	-map "$OUT.map" \
	$EXTRA_LD_FLAGS \
	$OBJS \
	-o "$OUT"

"$PYTHON" "$DIR/patch_macho.py" "$OUT"
"$PYTHON" "$DIR/verify.py" "$OUT"
