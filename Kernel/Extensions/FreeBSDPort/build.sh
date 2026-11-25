#!/bin/sh
set -eu

PORT="$(cd "$(dirname "$0")" && pwd)"
BUILD="$PORT/../../build"
XNU="$BUILD/../xnu"
OUT="$PORT/obj"
SHIM="$PORT/shim"
CC=${CC:-clang}
MIGSHIM="$BUILD/mig/shim"
MIGGEN="$BUILD/mig/gen"
CONFIG="$BUILD/config-include"

mkdir -p "$OUT"

if [ ! -d "$BUILD/driver/export-hdrs" ]; then
    sh "$BUILD/driver/export-headers.sh"
fi

EXP="$BUILD/driver/export-hdrs"

INC="-I$SHIM"
INC="$INC -I$PORT"
INC="$INC -I$CONFIG"
INC="$INC -I$XNU/bsd/machine -I$XNU/bsd/i386 -I$MIGGEN"
INC="$INC -I$EXP -I$XNU"
INC="$INC -I$XNU/osfmk -I$XNU/osfmk/i386"
INC="$INC -I$XNU/pexpert -I$XNU/pexpert/i386 -I$XNU/pexpert/gen"
INC="$INC -I$XNU/iokit -I$XNU/libkern -I$XNU/libkern/libc -I$XNU/libkern/c++"
INC="$INC -I$XNU/bsd -I$XNU/EXTERNAL_HEADERS -I$XNU/EXTERNAL_HEADERS/staging"
INC="$INC -I$MIGSHIM"

CFLAGS="-target x86_64-apple-darwin -std=gnu17 -Ulinux \
-Wno-deprecated-non-prototype -fno-builtin \
-Wno-macro-redefined -Wno-pointer-sign -Wno-duplicate-decl-specifier \
-Wno-incompatible-function-pointer-types -Wno-incompatible-pointer-types \
-D__XNU_PRIVATE_UNTRUSTED__ -DPRIVATE -DPRIVATE_EXTERNAL -D_MACHDEP \
-DBSD_COMPONENTS -DKERNEL -DKERNEL_PRIVATE -DXNU_KERNEL_PRIVATE \
-DMACH_KERNEL -DBSD_KERNEL_PRIVATE -DBSD_BUILD \
-DCONFIG_KERNEL -DKERNEL_BUILD -DAPPLE_ARCH=x86_64 -D__i386__ -D__x86_64__ \
-include kernel_config.h \
-include $CONFIG/kernel_base_types.h \
-include stddef.h \
-include $XNU/osfmk/kern/ledger.h \
-include $XNU/osfmk/kern/ipc_host.h \
-include $XNU/san/memory/memintrinsics.h \
-include skywalk/os_packet.h"

OBJS=""
FAIL=""

for f in virtio.c virtqueue.c virtio_pci.c if_vtnet.c; do
    if [ ! -f "$PORT/vtnet/$f" ]; then
        continue
    fi
    obj="$OUT/${f%.c}.o"
    if $CC $CFLAGS $INC -c "$PORT/vtnet/$f" -o "$obj" 2> "$OUT/$f.err"; then
        echo "OK: $f -> $obj"
        rm -f "$OUT/$f.err"
        OBJS="$OBJS $obj"
    else
        echo "FAIL: $f (see $OUT/$f.err)"
        FAIL="$FAIL $f"
    fi
done

if [ -n "$FAIL" ]; then
    echo "failed:$FAIL"
    exit 1
fi

echo "all driver objects built"
