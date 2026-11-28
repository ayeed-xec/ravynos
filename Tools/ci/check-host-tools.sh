#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# No GPL program is run by the build (ADR-17, SYSTEM-DESIGN.md 2.2, 5.6). The
# gate scans every script, workflow, Rust command and image recipe we own and
# fails on an invocation in command position. Vendored trees are not ours and
# are skipped.
#
# Three scopes keep the gate honest instead of noisy:
#   - the legacy list is matched loosely, as it always was;
#   - flex, lex, perl, apt-get, sed and awk count only at the start of a
#     command (line start, or after one of ; && || | $( ), because they also
#     appear inside prose messages;
#   - apt-get is allowed in .github/workflows, which provision a bare runner,
#     and cmake and python3 belong to the LLVM and Swift components.
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$root" || exit 1

legacy_deny='gcc|g\+\+|cc|c\+\+|cpp|bison|yacc|make|m4|patch|objcopy|objdump|readelf|ranlib|nm|ld|ar|gdb|autoconf|automake|libtool|gperf'
strict_deny='flex|lex|perl|apt-get|sed|awk'
component_deny='cmake|python3|python'

scripts=$(git ls-files -- '*.sh' '*.bash' '*.yml' '*.yaml' '*.mk' 'Makefile' 'Makefile.*' 'CMakeLists.txt' '*.cmake' 'Containerfile' ':!Kernel/xnu/**' ':!Tools/mig/upstream/**' ':!build/**' 2>/dev/null)
rs=$(git ls-files -- '*.rs' ':!Kernel/xnu/**' ':!Tools/mig/upstream/**' ':!build/**' 2>/dev/null)
if [ -z "$scripts" ] && [ -z "$rs" ]; then
    echo "error: git ls-files found no scripts to scan" >&2
    exit 1
fi

status=0
hits=""

# append_hits <file> <extended-regex>: file:line for every non-comment match.
# grep -H supplies the file prefix, so no re-writing is needed.
append_hits() {
    local file=$1 pattern=$2 out
    out=$(grep -HnE "$pattern" "$file" 2>/dev/null) || true
    [ -n "$out" ] || return 0
    out=$(printf '%s\n' "$out" | grep -vE '^[^:]+:[0-9]+:[[:space:]]*#') || true
    [ -n "$out" ] || return 0
    hits+=$out$'\n'
}

start='(^|[;&|]|\$\()[[:space:]]*(sudo[[:space:]]+)?'
command_only="^[[:space:]]*(sudo[[:space:]]+)?"

while IFS= read -r f; do
    [ -n "$f" ] || continue
    append_hits "$f" "(^|[|&;(]|\\\$\\(|[;[:space:]])(sudo[[:space:]]+)?(${legacy_deny})([[:space:]]|$)"

    case "$f" in
        .github/workflows/*) ;;
        *) append_hits "$f" "${start}(${strict_deny})([[:space:]]|$)" ;;
    esac

    case "$f" in
        Tools/toolchain/llvm/*|Tools/toolchain/swift/*|.github/workflows/stage1-llvm.yml) ;;
        *) append_hits "$f" "(${command_only}|[&|][&|][[:space:]]*)(sudo[[:space:]]+)?(${component_deny})([[:space:]]|$)" ;;
    esac
done <<< "$scripts"

while IFS= read -r f; do
    [ -n "$f" ] || continue
    append_hits "$f" "Command::new\(\"(${legacy_deny}|${strict_deny})\"\)"
done <<< "$rs"

if [ -n "$hits" ]; then
    printf '%s' "$hits" >&2
    echo "error: a GPL or restricted program is invoked from a build script (see lines above)" >&2
    status=1
fi

if [ ! -s Tools/mig/generated/lexxer.c ]; then
    echo "error: Tools/mig/generated/lexxer.c is missing; the mig build would need flex and m4, which are not allowed" >&2
    status=1
fi

if [ "$status" -eq 0 ]; then
    echo "host tool gate OK: no GPL program is invoked by the build"
fi
exit "$status"
