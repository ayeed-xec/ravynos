#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the VENDORS pin of one fetched tree as three lines: upstream, tag and
# commit. VENDORS is the one place a pin is written (SYSTEM-DESIGN.md S01); the
# fetch and the cache key both read it through this script.
#
# usage: vendor-pin.sh <name>

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <name>" >&2
    exit 2
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
want=$1
current="" upstream="" tag="" commit="" fetch=""
found=0

finish() {
    [ "$current" = "$want" ] || return 0
    found=1
    if [ "$fetch" != "true" ]; then
        echo "error: VENDORS entry $want is not fetch = true" >&2
        exit 1
    fi
    if [ -z "$upstream" ] || [ -z "$tag" ] || [ ${#commit} -ne 40 ]; then
        echo "error: VENDORS entry $want needs upstream, tag and a 40-hex commit" >&2
        exit 1
    fi
    printf '%s\n%s\n%s\n' "$upstream" "$tag" "$commit"
}

value_re='^([a-z]+)[[:space:]]*=[[:space:]]*"?([^"]*)"?[[:space:]]*$'
while IFS= read -r line || [ -n "$line" ]; do
    if [ "$line" = "[[vendor]]" ]; then
        finish
        current="" upstream="" tag="" commit="" fetch=""
        continue
    fi
    [[ $line =~ $value_re ]] || continue
    case ${BASH_REMATCH[1]} in
        name) current=${BASH_REMATCH[2]} ;;
        upstream) upstream=${BASH_REMATCH[2]} ;;
        tag) tag=${BASH_REMATCH[2]} ;;
        commit) commit=${BASH_REMATCH[2]} ;;
        fetch) fetch=${BASH_REMATCH[2]} ;;
    esac
done <"$ROOT/VENDORS"
finish

if [ "$found" -eq 0 ]; then
    echo "error: no VENDORS entry named $want" >&2
    exit 1
fi
