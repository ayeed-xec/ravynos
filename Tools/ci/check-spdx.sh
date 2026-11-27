#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Every file we own carries an SPDX licence identifier in its first two lines
# (GAP-45, GAP-57). Vendored trees keep their upstream headers and are checked by
# 'rv vendors check' instead, so they are out of scope here.
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$root" || exit 1

if ! command -v git >/dev/null 2>&1; then
    echo "error: git is required for the SPDX gate but was not found." >&2
    exit 1
fi

status=0
missing=0

while IFS= read -r file; do
    case "$file" in
        Kernel/xnu/*|Tools/mig/upstream/*|Tools/mig/tests/*|build/*) continue ;;
        *.c|*.h|*.cpp|*.rs|*.sh|*.bash|*.toml|*.yml|*.yaml|VENDORS) ;;
        *) continue ;;
    esac
    if ! head -2 -- "$file" | grep -q 'SPDX-License-Identifier:'; then
        echo "error: no SPDX licence identifier: $file"
        missing=$((missing + 1))
    fi
done < <(git ls-files)

if [ "$missing" -ne 0 ]; then
    echo "error: $missing file(s) we own lack an SPDX header." >&2
    echo "       add '# SPDX-License-Identifier: BSD-2-Clause' (or '//' for C and Rust)" >&2
    echo "       in the first two lines and commit the result." >&2
    status=1
fi

if [ "$status" -ne 0 ]; then
    echo "SPDX gate: FAILED"
    exit 1
fi
echo "SPDX gate: OK"
