#!/bin/sh
set -eu
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
OUT="$DIR/testimg"
CC=${CC:-gcc}
GUEST="$DIR/../link/guest_kernel"

if [ ! -f "$GUEST" ]; then
	echo "build.sh: $GUEST missing; run sh Kernel/build/link/stub/build.sh first" >&2
	exit 1
fi

rm -f "$OUT/test.iso"
mkdir -p "$OUT/isodir/boot/grub"

"$CC" -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -c \
	"$DIR/loader/stub.S" -o "$OUT/boot32.o"
"$CC" -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -Os -c \
	"$DIR/loader/stub.c" -o "$OUT/stub.o"
"$CC" -T "$DIR/loader/link64.ld" -o "$OUT/kernel.elf" -ffreestanding -m64 -nostdlib \
	-no-pie -Wl,--build-id=none "$OUT/boot32.o" "$OUT/stub.o" -lgcc
grub-file --is-x86-multiboot2 "$OUT/kernel.elf"

cp "$GUEST" "$OUT/isodir/boot/guest_kernel"
cat > "$OUT/isodir/boot/grub/grub.cfg" <<'CFG'
set timeout=0
menuentry "ravynOS" {
	multiboot2 /boot/kernel.elf
	module2 /boot/guest_kernel "guest_kernel"
	boot
}
CFG
cp "$OUT/kernel.elf" "$OUT/isodir/boot/kernel.elf"
grub-mkrescue -o "$OUT/test.iso" "$OUT/isodir" --compress=no || exit 1
echo "built $OUT/test.iso"
