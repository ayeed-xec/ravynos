#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Every CI gate of SYSTEM-DESIGN.md 5.6 except the boot smokes, in one place,
# meant to run inside the image built from Tools/ci/Containerfile. CI runs this
# script; so can anyone with the image:
#
#   docker run --rm -v "$PWD:/src" -w /src ravynos-gates Tools/ci/gates.sh <range>
#
# usage: gates.sh <commit-range>   (the commits whose subjects are checked)

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <commit-range>" >&2
    exit 2
fi
RANGE=$1
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

step() { printf '\n== %s\n' "$*"; }

step "Rust is the version rust-toolchain.toml names"
want=$(while IFS= read -r line; do
    case "$line" in
        'channel = "'*)
            line=${line#channel = \"}
            printf '%s\n' "${line%\"}"
            break
            ;;
    esac
done <rust-toolchain.toml)
have=$(rustc --version)
case "$have" in
    "rustc $want "*) echo "$have" ;;
    *)
        echo "error: image $have, rust-toolchain.toml $want" >&2
        exit 1
        ;;
esac

step "stage-1 build tools (byacc, unifdef, rvpatch, mkfat, elf2pe, mig)"
Tools/toolchain/buildtools.sh
bin=$(Tools/toolchain/buildtools.sh --print-bin)
PATH="$bin:$PATH"
for tool in byacc unifdef rvpatch mig; do
    [ "$(command -v "$tool")" = "$bin/$tool" ] || {
        echo "error: $tool resolves to $(command -v "$tool"), not stage 1" >&2
        exit 1
    }
done

step "lint the CI and boot scripts"
for f in Tools/ci/*.sh Tests/tools/run.sh Tests/rv/negative/run.sh Tests/boot/*.sh Boot/*.sh; do
    bash -n "$f"
done
shellcheck Tools/ci/*.sh Tests/tools/run.sh Tests/rv/negative/run.sh Tests/boot/*.sh Boot/*.sh

step "host-tool, SPDX, format and commit-subject gates"
Tools/ci/check-host-tools.sh
Tools/ci/check-spdx.sh
Tools/ci/check-format.sh
Tools/ci/check-crate-licenses.sh
Tools/ci/check-commit-subjects.sh "$RANGE"

step "host-tool tests"
Tests/tools/run.sh

step "rv: build, tests, lints"
cargo build --locked --manifest-path Tools/rv/Cargo.toml
cargo test --locked --manifest-path Tools/rv/Cargo.toml
cargo clippy --locked --manifest-path Tools/rv/Cargo.toml --all-targets -- -D warnings
RV="$ROOT/Tools/rv/target/debug/rv"
"$RV" test

step "commands whose tool is not built yet exit 3"
not_built() {
    local code=0
    "$RV" "$@" || code=$?
    [ "$code" -eq 3 ] || {
        echo "error: rv $* exited $code, expected 3 (E-NOT-BUILT)" >&2
        exit 1
    }
}
not_built kernelcache
not_built codesign
not_built symbolicate
not_built build -t riscv64-ravynos

step "vendored trees match their pins"
"$RV" vendors check

step "rv build and the S02 Mach-O assertion"
# hello and hello-cli are Mach-O files that make Linux system calls, not
# Darwin programs (GAP-07); this checks their format only.
for arch in x86_64 arm64; do
    "$RV" build -t "$arch-ravynos"
    Tools/ci/check-macho.sh "build/$arch-ravynos/libhello/libhello.dylib" "$arch" DYLIB
    Tools/ci/check-macho.sh "build/$arch-ravynos/hello/hello" "$arch" EXECUTE
    Tools/ci/check-macho.sh "build/$arch-ravynos/hello-cli/hello-cli" "$arch" EXECUTE
done

step "license audit and negative fixtures"
"$RV" audit license
RV="$RV" bash Tests/rv/negative/run.sh

step "mig from the pinned tree plus its patches"
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
Tools/mig/build.sh --out "$out" --smoke

step "xnuhook plans the byacc parsers (S05)"
cargo test --locked --manifest-path Tools/xnuhook/Cargo.toml
cargo clippy --locked --manifest-path Tools/xnuhook/Cargo.toml --all-targets -- -D warnings

printf '\nall gates passed\n'
