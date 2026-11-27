#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Fetch swiftlang/llvm-project at the commit pinned in Tools/toolchain/llvm/_PROVENANCE
# into build/vendor/llvm-project. The source is fetched at build time and never
# committed to the repository (SYSTEM-DESIGN.md S02: fetched tools live outside git).
#
# usage: fetch.sh [--refetch]
#
# Uses git only; the fetched tree is verified against the pinned commit and tag.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TREE="$(cd "$SCRIPT_DIR/../../.." && pwd)"
PIN="$SCRIPT_DIR/_PROVENANCE"
DEST="$TREE/build/vendor/llvm-project"

if [ "${1:-}" != "" ] && [ "$1" != "--refetch" ]; then
    echo "usage: $0 [--refetch]" >&2
    exit 2
fi

pin_line() {
    # pin_line <label>: read the value of a "* <label>: ..." line from _PROVENANCE
    local label=$1
    sed -n "s/^\\* ${label}: //p" "$PIN"
}

URL="$(pin_line 'Upstream URL')"
TAG="$(pin_line 'Tag')"
COMMIT="$(pin_line 'Commit')"
for v in "$URL" "$TAG" "$COMMIT"; do
    if [ -z "$v" ]; then
        echo "error: Tools/toolchain/llvm/_PROVENANCE is missing a required line (Upstream URL/Tag/Commit)" >&2
        exit 1
    fi
done

verify_tag_against_commit() {
    # The peeled tag object must resolve to the pinned commit, so the tag in the
    # provenance file cannot silently drift from the commit it claims to name.
    local peeled
    peeled=$(git ls-remote "$URL" "refs/tags/${TAG}^{}" | awk '/\^\{\}$/ {print $1; exit}')
    if [ -z "$peeled" ]; then
        peeled=$(git ls-remote --tags --refs "$URL" "refs/tags/${TAG}" | awk '{print $1}')
    fi
    if [ "$peeled" != "$COMMIT" ]; then
        echo "error: tag ${TAG} resolves to ${peeled:-<nothing>} upstream, not the pinned commit ${COMMIT}" >&2
        exit 1
    fi
}

verify_fetched() {
    [ -d "$DEST/.git" ] || return 1
    local head
    head=$(git -C "$DEST" rev-parse HEAD)
    [ "$head" = "$COMMIT" ] || return 1
    git -C "$DEST" diff --quiet -- 2>/dev/null || {
        echo "error: build/vendor/llvm-project has local modifications; refusing to build from it" >&2
        exit 1
    }
    return 0
}

if [ "${1:-}" = "--refetch" ]; then
    rm -rf "$DEST"
fi

if verify_fetched; then
    echo "llvm fetch: already at pinned commit ${COMMIT:0:12} in ${DEST#$TREE/}"
    exit 0
fi

rm -rf "$DEST"
mkdir -p "$(dirname "$DEST")"

echo "llvm fetch: verifying tag ${TAG} against pinned commit"
verify_tag_against_commit

echo "llvm fetch: shallow-cloning ${URL} at ${COMMIT:0:12} into ${DEST#$TREE/}"
git init -q "$DEST"
git -C "$DEST" remote add origin "$URL"
git -C "$DEST" fetch -q --depth 1 origin "$COMMIT"
git -C "$DEST" checkout -q --detach FETCH_HEAD

if ! verify_fetched; then
    echo "error: fetched tree does not match the pinned commit ${COMMIT}" >&2
    exit 1
fi

echo "llvm fetch: fetched $(git -C "$DEST" rev-parse HEAD) (tag ${TAG})"
