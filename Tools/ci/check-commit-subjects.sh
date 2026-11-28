#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Check the commit subjects in a revision range (GAP-59, SYSTEM-DESIGN.md 2.7):
# each subject is LIMIT characters or fewer and starts with a lowercase scope,
# so a branch cannot drift back to mixed `Docs:`/`docs:` prefixes.
#
# usage: check-commit-subjects.sh [<range>]
#   default: <upstream>..HEAD when an upstream is set, else HEAD^..HEAD, so the
#   check looks at the commits being offered, not the imported upstream history.

set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
LIMIT=72

if [ $# -gt 0 ]; then
    RANGE=$1
elif git -C "$ROOT" rev-parse --verify --quiet '@{upstream}' >/dev/null; then
    RANGE='@{upstream}..HEAD'
else
    RANGE='HEAD^..HEAD'
fi

fail=0
while IFS= read -r line; do
    sha=${line%% *}
    subject=${line#* }
    if [ ${#subject} -gt $LIMIT ]; then
        printf 'error: %s: subject is %s characters, limit is %s\n' \
            "${sha:0:10}" "${#subject}" "$LIMIT" >&2
        fail=1
    fi
    case $subject in
        [a-z]*": "*) ;;
        *)
            printf 'error: %s: subject has no lowercase scope: %s\n' "${sha:0:10}" "$subject" >&2
            fail=1
            ;;
    esac
done < <(git -C "$ROOT" log --format='%H %s' "$RANGE")

if [ $fail -eq 0 ]; then
    printf 'commit subjects OK: %s\n' "$RANGE"
fi
exit $fail
