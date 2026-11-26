#!/bin/sh
set -eu

# Usage: sh Kernel/build/driver/compile.sh [-c] [ARCH] [files...]
#   ARCH = x86_64 (default) | arm64 | riscv64; also honored as env var ARCH.

DRV="$(cd "$(dirname "$0")" && pwd)"
BUILD="$(cd "$DRV/.." && pwd)"
XNU="$(cd "$BUILD/../xnu" && pwd)"
CC=${CC:-clang}

MODE=syntax
if [ "${1:-}" = "-c" ]; then
	MODE=obj
	shift
fi

ARCH=${ARCH:-}
case "${1:-}" in
	x86_64|arm64|riscv64)
		ARCH=$1
		shift
		;;
esac
ARCH=${ARCH:-x86_64}

case "$ARCH" in
	x86_64)
		TARGET=x86_64-apple-darwin
		ARCHDEFS="-DAPPLE_ARCH=x86_64 -D__i386__ -D__x86_64__ -Di386 -Dx86_64"
		BSD_ARCH_INC="-I$XNU/bsd/i386"
		ARCH_EARLY_INC=""
		OSFMK_LATE_INC="-I$XNU/osfmk/i386"
		PEXPERT_LATE_INC="-I$XNU/pexpert/i386"
		;;
	arm64)
		TARGET=arm64-apple-darwin
		ARCHDEFS="-DAPPLE_ARCH=arm64 -D__arm64__ -D__aarch64__ -Darm64 -DARM64 -D__ARM64__ -DLP64"
		BSD_ARCH_INC="-I$XNU/bsd/arm"
		ARCH_EARLY_INC="-I$XNU/osfmk/arm64 -I$XNU/pexpert/arm"
		OSFMK_LATE_INC=""
		PEXPERT_LATE_INC=""
		;;
	riscv64)
		TARGET=riscv64-ravynos
		ARCHDEFS="-DAPPLE_ARCH=riscv64 -D__riscv"
		BSD_ARCH_INC=""
		ARCH_EARLY_INC=""
		OSFMK_LATE_INC=""
		PEXPERT_LATE_INC=""
		;;
	*)
		echo "compile.sh: unsupported ARCH '$ARCH' (use x86_64, arm64 or riscv64)" >&2
		exit 2
		;;
esac

OUT=${OUT:-"$DRV/export-hdrs"}
[ -d "$OUT" ] || "$DRV/export-headers.sh"

if [ ! -f "$OUT/libkern/version.h" ]; then
	mkdir -p "$OUT/libkern"
	sed -e 's/###KERNEL_VERSION_MAJOR###/12377/' \
		-e 's/###KERNEL_VERSION_MINOR###/121/' \
		-e 's/###KERNEL_VERSION_VARIANT###/6/' \
		-e 's/###KERNEL_VERSION_REVISION###/6/' \
		-e 's/###KERNEL_VERSION_STAGE###/VERSION_STAGE_RELEASE/' \
		-e 's/###KERNEL_VERSION_PRERELEASE_LEVEL###/0/' \
		-e 's/###KERNEL_BUILD_CONFIG###/RELEASE/' \
		-e 's/###KERNEL_VERSION_LONG###/12377.121.6/' \
		"$XNU/libkern/libkern/version.h.template" > "$OUT/libkern/version.h"
fi

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
		"$XNU/osfmk/kern/startup.c" \
		"$XNU/bsd/sys/systm.h" \
		"$XNU/bsd/sys/param.h" \
		"$XNU/bsd/sys/proc.h" \
		"$XNU/bsd/sys/lock.h" \
		"$XNU/bsd/sys/kernel.h" \
		"$XNU/bsd/sys/malloc.h" \
		"$XNU/bsd/net/kpi_interface.h"
fi

fail=0
for f in "$@"; do
	case "$f" in
		/*) ;;
		*) f="$PWD/$f" ;;
	esac
	echo "== $f"
	case "$f" in
		*/osfmk/vm/*)
			PRIV="-DMACH_KERNEL_PRIVATE -DMACH_KERNEL -DVM_UNSAFE_TYPES"
			;;
		*/osfmk/*|*/iokit/*)
			PRIV="-DMACH_KERNEL_PRIVATE -DMACH_KERNEL"
			;;
		*/pexpert/*)
			PRIV="-DMACH_KERNEL -DPEXPERT_KERNEL_PRIVATE"
			;;
		*/bsd/*)
			PRIV="-DMACH_KERNEL -DBSD_KERNEL_PRIVATE -DBSD_BUILD"
			;;
		*)
			PRIV=""
			;;
	esac
	case "$f" in
		*/bsd/*|*/osfmk/vm/*)
			SKYWALK_INC="-include skywalk/packet/os_packet.h"
			;;
		*)
			SKYWALK_INC=""
			;;
	esac
	case "$f" in
		*/osfmk/kern/locks.c|*/osfmk/kern/lock_mtx.c|*/osfmk/kern/lock_rw.c|\
*/osfmk/kern/lock_rw_old.c|*/osfmk/kern/lock_ticket.c|*/osfmk/kern/lock_ptr.c|\
*/osfmk/kern/lock_group.c|*/osfmk/i386/locks_i386.c|*/osfmk/i386/locks_i386_opt.c|\
*/osfmk/i386/lock_ticket_pv.c|*/osfmk/vm/vm_entry_lock.c)
			PRIV="$PRIV -DLOCK_PRIVATE"
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
	case "$f" in
		*.cpp|*.cc|*.cxx|*.hpp)
			STDLANG="-std=gnu++17"
			;;
		*)
			STDLANG="-std=gnu17"
			;;
	esac
	# shellcheck disable=SC2086
	if ! "$CC" -target "$TARGET" $ACTION \
		$STDLANG -Ulinux \
		-Wno-deprecated-non-prototype \
		-fno-builtin \
		-D__XNU_PRIVATE_UNTRUSTED__ \
		-DPRIVATE -DPRIVATE_EXTERNAL -D_MACHDEP -DBSD_COMPONENTS \
		-DKERNEL -DKERNEL_PRIVATE -DXNU_KERNEL_PRIVATE \
		$PRIV \
		-DCONFIG_KERNEL -DKERNEL_BUILD \
		$ARCHDEFS \
		-DKERNEL_BASE_OFFSET=0x100000 \
		-include "$CONFIG/kernel_config.h" \
		-include "$CONFIG/mach_kdp.h" \
		-include "$CONFIG/kernel_base_types.h" \
		-include "$XNU/bsd/sys/_types/_caddr_t.h" \
		-include stddef.h \
		-include "$XNU/osfmk/kern/ledger.h" \
		-include "$XNU/osfmk/kern/ipc_host.h" \
		-include "$XNU/san/memory/memintrinsics.h" \
		$SKYWALK_INC \
		"-I$CONFIG" \
		"-I$XNU/bsd/machine" \
		$BSD_ARCH_INC \
		"-I$MIGGEN" \
		$ARCH_EARLY_INC \
		"-I$OUT" \
		"-I$XNU" \
		"-I$XNU/osfmk" \
		$OSFMK_LATE_INC \
		"-I$XNU/pexpert" \
		$PEXPERT_LATE_INC \
		"-I$XNU/pexpert/gen" \
		"-I$XNU/iokit" \
		"-I$XNU/libkern" \
		"-I$XNU/libkern/libc" \
		"-I$XNU/libkern/c++" \
		"-I$XNU/bsd" \
		"-I$XNU/EXTERNAL_HEADERS" \
		"-I$XNU/EXTERNAL_HEADERS/staging" \
		"-I$MIGSHIM" \
		"-I$DRV/shim" \
		"$f"
	then
		fail=$((fail + 1))
	fi
done
exit $fail
