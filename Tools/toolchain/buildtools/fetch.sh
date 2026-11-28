#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Fetch one `fetch = true` tree from VENDORS into build/vendor/<name> at its
# pinned commit. The tree is never committed; a checkout that is at another
# commit or has local edits is refetched or refused.
#
# usage: fetch.sh <name> [--refetch]

set -euo pipefail

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ] || { [ "$#" -eq 2 ] && [ "$2" != "--refetch" ]; }; then
    echo "usage: $0 <name> [--refetch]" >&2
    exit 2
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
NAME=$1
DEST="$ROOT/build/vendor/$NAME"

mapfile -t pin < <(bash "$SCRIPT_DIR/vendor-pin.sh" "$NAME")
[ "${#pin[@]}" -eq 3 ] || exit 1
URL=${pin[0]} TAG=${pin[1]} COMMIT=${pin[2]}

at_pin() {
    [ -d "$DEST/.git" ] && [ "$(git -C "$DEST" rev-parse HEAD)" = "$COMMIT" ]
}

if [ "${2:-}" = "--refetch" ]; then
    rm -rf "$DEST"
fi

if at_pin; then
    # Untracked and ignored files count too: a header dropped into the tree
    # would change the build without changing the commit.
    if [ -n "$(git -C "$DEST" status --porcelain --ignored)" ]; then
        echo "error: ${DEST#"$ROOT"/} has local changes; refusing to build from it (--refetch)" >&2
        exit 1
    fi
    echo "fetch $NAME: already at ${COMMIT:0:12}"
    exit 0
fi

# The tag must still name the pinned commit upstream, so the pin cannot claim a
# release it is not. Annotated tags are peeled; lightweight tags are not.
peeled=$(git ls-remote "$URL" "refs/tags/$TAG^{}" "refs/tags/$TAG" |
    while read -r sha ref; do
        if [ "$ref" = "refs/tags/$TAG^{}" ]; then echo "$sha"; fi
    done)
if [ -z "$peeled" ]; then
    peeled=$(git ls-remote "$URL" "refs/tags/$TAG" | cut -f1)
fi
if [ "$peeled" != "$COMMIT" ]; then
    echo "error: $NAME tag $TAG is ${peeled:-<missing>} upstream, VENDORS pins $COMMIT" >&2
    exit 1
fi

rm -rf "$DEST"
mkdir -p "$DEST"
git init -q "$DEST"
git -C "$DEST" fetch -q --depth 1 "$URL" "$COMMIT"
git -C "$DEST" checkout -q --detach FETCH_HEAD
at_pin || {
    echo "error: fetched $NAME is not at $COMMIT" >&2
    exit 1
}
echo "fetch $NAME: $TAG at ${COMMIT:0:12}"
