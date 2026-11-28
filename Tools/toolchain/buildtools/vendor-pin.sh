#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Print the VENDORS pin of one fetched tree as three lines: upstream, tag and
# commit. The build-tools fetch and cache key read their pins through this
# script. (The LLVM pin is also written in Tools/toolchain/llvm/_PROVENANCE,
# which llvm/fetch.sh reads; Tests/tools/run.sh checks that the two agree.)
#
# VENDORS is TOML, and rv parses it as such. This reader accepts only the
# subset the file uses: comments, blank lines, [[vendor]] headers and one-line
# `key = value` pairs. Anything else is an error rather than a line skipped.
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
lineno=0

die() {
    echo "error: VENDORS:$lineno: $1" >&2
    exit 1
}

finish() {
    [ "$current" = "$want" ] || return 0
    [ "$found" -eq 0 ] || die "two entries are named $want"
    found=1
    [ "$fetch" = "true" ] || die "entry $want is not fetch = true"
    if [ -z "$upstream" ] || [ -z "$tag" ]; then die "entry $want needs upstream and tag"; fi
    [[ $commit =~ ^[0-9a-f]{40}$ ]] || die "entry $want: commit '$commit' is not 40 hex digits"
    pin=("$upstream" "$tag" "$commit")
}

pair_re='^([a-z_]+)[[:space:]]*=[[:space:]]*("([^"]*)"|true|false|\[[^]]*\])[[:space:]]*$'
in_entry=0
while IFS= read -r line || [ -n "$line" ]; do
    lineno=$((lineno + 1))
    case "$line" in
        "" | "#"*) continue ;;
        "[[vendor]]")
            finish
            current="" upstream="" tag="" commit="" fetch=""
            in_entry=1
            continue
            ;;
    esac
    [ "$in_entry" -eq 1 ] || die "unexpected line before the first [[vendor]]: $line"
    [[ $line =~ $pair_re ]] || die "cannot read: $line"
    key=${BASH_REMATCH[1]}
    value=${BASH_REMATCH[3]:-${BASH_REMATCH[2]}}
    case $key in
        name) current=$value ;;
        upstream) upstream=$value ;;
        tag) tag=$value ;;
        commit) commit=$value ;;
        fetch) fetch=$value ;;
    esac
done <"$ROOT/VENDORS"
lineno=$((lineno + 1))
finish

[ "$found" -eq 1 ] || {
    echo "error: no VENDORS entry named $want" >&2
    exit 1
}
printf '%s\n' "${pin[@]}"
