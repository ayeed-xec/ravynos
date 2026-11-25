#!/bin/sh
set -eu

DRV="$(cd "$(dirname "$0")" && pwd)"
BUILD="$(cd "$DRV/.." && pwd)"
XNU="$(cd "$BUILD/../xnu" && pwd)"
CC=${CC:-clang}

MODE=syntax
if [ "${1:-}" = "-c" ]; then
	MODE=obj
	shift
fi

OUT=${OUT:-"$DRV/export-hdrs"}
[ -d "$OUT" ] || "$DRV/export-headers.sh"

MIGSHIM="$BUILD/mig/shim"
MIGGEN="$BUILD/mig/gen"
CONFIG="$BUILD/config-include"
OBJDIR="$BUILD/kernel-objs"

# No args: syntax-check the core osfmk header surface.
if [ $# -eq 0 ]; then
	set -- \
		"$XNU/osfmk/kern/assert.h" \
		"$XNU/osfmk/kern/ast.h" \
		"$XNU/osfmk/kern/queue.h" \
		"$XNU/osfmk/kern/misc_protos.h" \
		"$XNU/osfmk/kern/locks.h" \
		"$XNU/osfmk/machine/machine_routines.h" \
		"$XNU/osfmk/kern/zalloc.h" \
		"$XNU/osfmk/kern/task.h" \
		"$XNU/osfmk/kern/thread.h" \
		"$XNU/osfmk/kern/startup.c"
fi

for f in "$@"; do
	case "$f" in
		/*) ;;
		*) f="$PWD/$f" ;;
	esac
	echo "== $f"
	case "$f" in
		*/osfmk/*|*/iokit/*)
			PRIV="-DMACH_KERNEL_PRIVATE -DMACH_KERNEL"
			;;
		*/bsd/*)
			PRIV="-DMACH_KERNEL -DBSD_KERNEL_PRIVATE -DBSD_BUILD"
			;;
		*)
			PRIV=""
			;;
	esac
	case "$MODE" in
		obj)
			rel=${f#"$XNU/"}
			obj="$OBJDIR/${rel%.c}.o"
			mkdir -p "$(dirname "$obj")"
			ACTION="-c -o $obj"
			;;
		syntax)
			ACTION="-fsyntax-only"
			;;
	esac
	"$CC" -target x86_64-apple-darwin $ACTION \
		-std=gnu17 -Ulinux \
		-Wno-deprecated-non-prototype \
		-fno-builtin \
		-D__XNU_PRIVATE_UNTRUSTED__ \
		-DPRIVATE -DPRIVATE_EXTERNAL -D_MACHDEP -DBSD_COMPONENTS \
		-DKERNEL -DKERNEL_PRIVATE -DXNU_KERNEL_PRIVATE \
		$PRIV \
		-DCONFIG_KERNEL -DKERNEL_BUILD \
		-DAPPLE_ARCH=x86_64 -D__i386__ -D__x86_64__ \
		-include "$CONFIG/kernel_config.h" \
		-include "$CONFIG/kernel_base_types.h" \
		-include stddef.h \
		-include "$XNU/osfmk/kern/ledger.h" \
		-include "$XNU/osfmk/kern/ipc_host.h" \
		-include "$XNU/san/memory/memintrinsics.h" \
		-include skywalk/os_packet.h \
		"-I$CONFIG" \
		"-I$XNU/bsd/machine" \
		"-I$XNU/bsd/i386" \
		"-I$MIGGEN" \
		"-I$OUT" \
		"-I$XNU" \
		"-I$XNU/osfmk" \
		"-I$XNU/osfmk/i386" \
		"-I$XNU/pexpert" \
		"-I$XNU/pexpert/i386" \
		"-I$XNU/pexpert/gen" \
		"-I$XNU/iokit" \
		"-I$XNU/libkern" \
		"-I$XNU/libkern/libc" \
		"-I$XNU/libkern/c++" \
		"-I$XNU/bsd" \
		"-I$XNU/EXTERNAL_HEADERS" \
		"-I$XNU/EXTERNAL_HEADERS/staging" \
		"-I$MIGSHIM" \
		"$f"
done
