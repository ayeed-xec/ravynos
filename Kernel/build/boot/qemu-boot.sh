#!/bin/sh
set -eu
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
OUT="$DIR/testimg"
LOG="$OUT/serial.log"
QEMU=${QEMU:-qemu-system-x86_64}

rm -f "$LOG"
"$QEMU" -machine q35 -m 512 -display none -no-reboot \
	-cdrom "$OUT/test.iso" -serial file:"$LOG" -daemonize -pidfile "$OUT/qemu.pid"

i=0
while [ $i -lt 240 ]; do
	[ -f "$LOG" ] && grep -q "Mach-O link OK" "$LOG" && break
	if [ -f "$OUT/qemu.pid" ] && ! kill -0 "$(cat "$OUT/qemu.pid")" 2>/dev/null; then
		echo "qemu-boot.sh: QEMU exited before the Mach-O link banner" >&2
		break
	fi
	i=$((i + 1))
	sleep 0.5
done
if [ -f "$OUT/qemu.pid" ]; then
	QPID=$(cat "$OUT/qemu.pid")
	kill "$QPID" 2>/dev/null || true
	wait "$QPID" 2>/dev/null || true
	rm -f "$OUT/qemu.pid"
fi
cat "$LOG"
check_banner() {
	grep -q "$1" "$LOG" || {
		echo "qemu-boot.sh: banner missing from serial log: $1" >&2
		exit 1
	}
}
check_banner "RAVYNOS-BOOT-OK WP-A2 macho-loader long-mode"
check_banner "grub magic=0x0*36d76289"
check_banner "macho-loader: jumping to guest kernel"
check_banner "ravynOS WP-A2b kernel stub: Mach-O link OK"
echo "QEMU-BOOT-OK"
