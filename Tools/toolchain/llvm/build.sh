#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors

# Build the stage-1 toolchain from the fetched swiftlang/llvm-project tree:
# clang, lld (all flavors, ld64.lld included), llvm-ar, llvm-nm, llvm-symbolizer
# and llvm-strip. The host compiler is clang itself; cmake and ninja drive the
# build. No gcc, no binutils, no GNU make anywhere in the chain
# (SYSTEM-DESIGN.md S02 stage-1/D8, section 2.2 tooling boundary).
#
# usage: build.sh <src-dir> <install-dir> [<jobs>]
#
# <src-dir> is a fetched llvm-project tree (Tools/toolchain/llvm/fetch.sh).
# <install-dir> receives bin/ with the stage-1 tools and tools.sha256 with the
# content hash of every installed binary.

set -euo pipefail

if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
    echo "usage: $0 <src-dir> <install-dir> [<jobs>]" >&2
    exit 2
fi

SRC=$(cd "$1" && pwd)
mkdir -p "$2"
INSTALL_DIR=$(cd "$2" && pwd -P)
if [ "$#" -eq 3 ]; then
    JOBS=$3
else
    JOBS=$(nproc)
fi

# The stage-1 tool set. These targets are what stage 2 and the rv build need;
# building exactly these keeps the build small and the provenance tight.
TARGETS=(clang lld llvm-ar llvm-nm llvm-symbolizer llvm-strip)
INSTALLED=(clang clang++ lld ld.lld ld64.lld lld-link wasm-ld llvm-ar llvm-nm llvm-symbolizer llvm-strip)

for tool in cmake ninja clang clang++; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "error: $tool is required for the stage-1 build but was not found on PATH" >&2
        exit 1
    fi
done
for tool in git; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "error: $tool is required for the toolchain work but was not found on PATH" >&2
        exit 1
    fi
done

[ -d "$SRC/llvm" ] && [ -d "$SRC/clang" ] && [ -d "$SRC/lld" ] || {
    echo "error: $SRC does not look like a fetched llvm-project tree; run Tools/toolchain/llvm/fetch.sh first" >&2
    exit 1
}

OBJ="$INSTALL_DIR/obj"
LOG="$INSTALL_DIR/build.log"
mkdir -p "$OBJ"

echo "llvm build: configuring (clang $(clang -dumpversion), ninja, $JOBS jobs)"
{
    cmake -G Ninja -S "$SRC/llvm" -B "$OBJ" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=clang \
        -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_C_FLAGS="-g0" \
        -DCMAKE_CXX_FLAGS="-g0" \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DLLVM_ENABLE_PROJECTS="clang;lld" \
        -DLLVM_TARGETS_TO_BUILD="X86;AArch64;RISCV" \
        -DLLVM_DISTRIBUTION_COMPONENTS="clang;lld;llvm-ar;llvm-nm;llvm-symbolizer;llvm-strip" \
        -DLLVM_APPEND_VC_REV=OFF \
        -DLLVM_ENABLE_BINDINGS=OFF \
        -DLLVM_BUILD_RUNTIME=OFF \
        -DLLVM_BUILD_LLVM_DYLIB=OFF \
        -DLLVM_INCLUDE_TESTS=OFF \
        -DLLVM_INCLUDE_BENCHMARKS=OFF \
        -DLLVM_INCLUDE_EXAMPLES=OFF \
        -DLLVM_INCLUDE_DOCS=OFF \
        -DLLVM_ENABLE_ASSERTIONS=OFF \
        -DLLVM_USE_SPLIT_DWARF=OFF \
        -DLLVM_PARALLEL_COMPILE_JOBS="$JOBS" \
        -DLLVM_PARALLEL_LINK_JOBS=1
} >"$LOG" 2>&1

echo "llvm build: compiling stage-1 targets (${TARGETS[*]}; log: ${LOG#$INSTALL_DIR/})"
cmake --build "$OBJ" --parallel "$JOBS" --target "${TARGETS[@]}" >>"$LOG" 2>&1

echo "llvm build: installing into ${INSTALL_DIR}/bin"
cmake --build "$OBJ" --target install-distribution >>"$LOG" 2>&1

# Strip with the llvm-strip we just built, never binutils strip.
if [ -x "$INSTALL_DIR/bin/llvm-strip" ]; then
    "$INSTALL_DIR/bin/llvm-strip" "$INSTALL_DIR"/bin/clang "$INSTALL_DIR"/bin/clang++
fi

echo "llvm build: hashing installed tools"
( cd "$INSTALL_DIR/bin" && sha256sum "${INSTALLED[@]}" ) >"$INSTALL_DIR/tools.sha256"

# Prove the tools run and are the versions we just built.
( cd "$INSTALL_DIR/bin" && ./clang --version && ./ld.lld --version && ./llvm-ar --version && ./llvm-nm --version && ./llvm-symbolizer --version ) >>"$LOG" 2>&1

for tool in "${INSTALLED[@]}"; do
    [ -x "$INSTALL_DIR/bin/$tool" ] || {
        echo "error: stage-1 build did not produce $tool (see $LOG)" >&2
        exit 1
    }
done

echo "llvm build: stage-1 tools built: ${INSTALLED[*]}"
