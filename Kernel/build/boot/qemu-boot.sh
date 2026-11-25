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
	[ -f "$LOG" ] && grep -q "grub magic" "$LOG" && break
	i=$((i + 1))
	sleep 0.5
done
kill "$(cat "$OUT/qemu.pid")" 2>/dev/null || true
wait "$(cat "$OUT/qemu.pid")" 2>/dev/null || true
rm -f "$OUT/qemu.pid"
cat "$LOG"
grep -q "RAVYNOS-BOOT-OK WP-A2c multiboot2 COM1" "$LOG"
grep -q "grub magic=0x36d76289" "$LOG"
echo "QEMU-BOOT-OK"
