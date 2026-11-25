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
while [ $i -lt 60 ]; do
	[ -f "$LOG" ] && grep -q "Mach-O link OK" "$LOG" && break
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
grep -q "RAVYNOS-BOOT-OK WP-A2 macho-loader long-mode" "$LOG"
grep -q "grub magic=0x0*36d76289" "$LOG"
grep -q "macho-loader: jumping to guest kernel" "$LOG"
grep -q "ravynOS WP-A2b kernel stub: Mach-O link OK" "$LOG"
echo "QEMU-BOOT-OK"
