#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$root" || exit 1

deny='gcc|g\+\+|cc|c\+\+|cpp|bison|yacc|make|m4|patch|objcopy|objdump|readelf|ranlib|nm|ld|ar|gdb|autoconf|automake|libtool|gperf'

files=$(git ls-files -- ':(glob)build/**/*.sh' ':(glob)tests/**/*.sh' ':(glob).github/**/*.yml' ':(glob).github/**/*.yaml' 2>/dev/null)
rs=$(git ls-files -- ':(glob)tools/rv/src/**/*.rs' 2>/dev/null)
if [ -z "$files" ] || [ -z "$rs" ]; then
    echo "error: git ls-files found no scripts to scan" >&2
    exit 1
fi

status=0
hits=$(printf '%s\n' "$files" | while IFS= read -r f; do
    [ -n "$f" ] || continue
    grep -nE "(^|[|&;(]|\\\$\(|[;[:space:]])(sudo[[:space:]]+)?(${deny})([[:space:]]|$)" "$f" |
        grep -vE "^[0-9]+:[[:space:]]*#" |
        sed "s|^|$f:|"
done)
hits="$hits$(printf '%s\n' "$rs" | while IFS= read -r f; do
    [ -n "$f" ] || continue
    grep -nE "Command::new\(\"(${deny})\"\)" "$f" | sed "s|^|$f:|"
done)"
hits=$(printf '%s\n' "$hits" | sed '/^$/d')

if [ -n "$hits" ]; then
    printf '%s\n' "$hits" >&2
    echo "error: a GPL program is invoked from a build script (see lines above)" >&2
    status=1
fi

if [ ! -s build/mig/generated/lexxer.c ]; then
    echo "error: build/mig/generated/lexxer.c is missing; the mig build would need flex and m4" >&2
    status=1
fi

if [ "$status" -eq 0 ]; then
    echo "host tool gate OK: no GPL program is invoked by the build"
fi
exit "$status"
