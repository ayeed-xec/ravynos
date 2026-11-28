#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Generate the XNU parsers that upstream ships as Bison output (ADR-17).
# The shipped copies are pruned from the vendored tree, so the files written
# here are the only ones the kernel build may compile.
#
# usage: gen-parsers.sh <outdir>
#
# Writes:
#   <outdir>/OSUnserialize.cpp                       from libkern/c++/OSUnserialize.y
#   <outdir>/OSUnserializeXMLSharedImplementation.h  from libkern/c++/OSUnserializeXML.y
#
# The XML parser header is included from both libkern/c++ and iokit/System, so
# the build puts this one output on both include paths.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
XNU="$(cd "$SCRIPT_DIR/../xnu" && pwd)"
BYACC=${BYACC:-byacc}

if [ $# -ne 1 ]; then
	echo "usage: gen-parsers.sh <outdir>" >&2
	exit 2
fi
OUT=$1

command -v "$BYACC" >/dev/null 2>&1 || {
	echo "error: $BYACC is required to regenerate the XNU parsers" >&2
	exit 1
}

# A pruned copy reappearing means the vendored tree was edited instead of
# regenerated; rv vendors check enforces the same list from VENDORS.
pruned=(
	libkern/c++/OSUnserialize.cpp
	iokit/System/OSUnserializeXMLSharedImplementation.h
	libkern/libkern/c++/OSUnserializeXMLSharedImplementation.h
)
for stale in "${pruned[@]}"; do
	if [ -e "$XNU/$stale" ]; then
		echo "error: $XNU/$stale is Bison output and must stay pruned (ADR-17)" >&2
		exit 1
	fi
done

mkdir -p "$OUT"

# Both grammars declare %pure_parser, so byacc needs -P for the state argument.
"$BYACC" -P -o "$OUT/OSUnserialize.cpp" "$XNU/libkern/c++/OSUnserialize.y"
"$BYACC" -P -o "$OUT/OSUnserializeXMLSharedImplementation.h" \
	"$XNU/libkern/c++/OSUnserializeXML.y"

for f in OSUnserialize.cpp OSUnserializeXMLSharedImplementation.h; do
	if [ ! -s "$OUT/$f" ]; then
		echo "error: $BYACC produced no $f" >&2
		exit 1
	fi
	printf 'generated %s\n' "$OUT/$f"
done
