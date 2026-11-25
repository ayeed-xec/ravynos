#!/bin/sh
# Same gate as qemu-boot.sh but through OVMF: the ISO is grub-mkrescue's
# hybrid image, so OVMF boots ./efi/boot/bootx64.efi and GRUB still runs
# the same multiboot2 config. Proves the loader under UEFI firmware
# (P2 exit names q35/OVMF).
set -eu
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
OUT="$DIR/testimg"
LOG="$OUT/serial-ovmf.log"
QEMU=${QEMU:-qemu-system-x86_64}
OVMF_CODE=${OVMF_CODE:-/usr/share/OVMF/OVMF_CODE_4M.fd}
OVMF_VARS_SRC=${OVMF_VARS_SRC:-/usr/share/OVMF/OVMF_VARS_4M.fd}

[ -f "$OVMF_CODE" ] || { echo "qemu-boot-ovmf.sh: no OVMF firmware at $OVMF_CODE" >&2; exit 1; }

rm -f "$LOG" "$OUT/OVMF_VARS.fd"
cp "$OVMF_VARS_SRC" "$OUT/OVMF_VARS.fd"

"$QEMU" -machine q35 -m 512 -cpu Penryn -display none -no-reboot \
	-drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
	-drive if=pflash,format=raw,file="$OUT/OVMF_VARS.fd" \
	-cdrom "$OUT/test.iso" -serial file:"$LOG" -daemonize -pidfile "$OUT/qemu-ovmf.pid"

i=0
while [ $i -lt 240 ]; do
	[ -f "$LOG" ] && grep -q "Mach-O link OK" "$LOG" && break
	if [ -f "$OUT/qemu-ovmf.pid" ] && ! kill -0 "$(cat "$OUT/qemu-ovmf.pid")" 2>/dev/null; then
		echo "qemu-boot-ovmf.sh: QEMU exited before the Mach-O link banner" >&2
		break
	fi
	i=$((i + 1))
	sleep 0.5
done
if [ -f "$OUT/qemu-ovmf.pid" ]; then
	QPID=$(cat "$OUT/qemu-ovmf.pid")
	kill "$QPID" 2>/dev/null || true
	wait "$QPID" 2>/dev/null || true
	rm -f "$OUT/qemu-ovmf.pid"
fi
cat "$LOG"
check_banner() {
	grep -q "$1" "$LOG" || {
		echo "qemu-boot-ovmf.sh: banner missing from serial log: $1" >&2
		exit 1
	}
}
check_banner "RAVYNOS-BOOT-OK WP-A2 macho-loader long-mode"
check_banner "macho-loader: jumping to guest kernel"
check_banner "ravynOS WP-A2b kernel stub: Mach-O link OK"
echo "QEMU-BOOT-OVMF-OK"
