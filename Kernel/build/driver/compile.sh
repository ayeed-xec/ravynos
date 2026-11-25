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
		"$XNU/osfmk/kern/startup.c" \
		"$XNU/bsd/sys/systm.h" \
		"$XNU/bsd/sys/param.h" \
		"$XNU/bsd/sys/proc.h" \
		"$XNU/bsd/sys/lock.h" \
		"$XNU/bsd/sys/kernel.h" \
		"$XNU/bsd/sys/malloc.h" \
		"$XNU/bsd/net/kpi_interface.h"
fi

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
			SKYWALK_INC="-include skywalk/os_packet.h"
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
	"$CC" -target x86_64-apple-darwin $ACTION \
		$STDLANG -Ulinux \
		-Wno-deprecated-non-prototype \
		-fno-builtin \
		-D__XNU_PRIVATE_UNTRUSTED__ \
		-DPRIVATE -DPRIVATE_EXTERNAL -D_MACHDEP -DBSD_COMPONENTS \
		-DKERNEL -DKERNEL_PRIVATE -DXNU_KERNEL_PRIVATE \
		$PRIV \
		-DCONFIG_KERNEL -DKERNEL_BUILD \
		-DAPPLE_ARCH=x86_64 -D__i386__ -D__x86_64__ -Di386 -Dx86_64 \
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
