#!/bin/sh
set -eu
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
OUT="$DIR/testimg"
CC=${CC:-clang}

mkdir -p "$OUT/isodir/boot/grub"
"$CC" -c "$DIR/loader/boot32.S" -o "$OUT/boot32.o" -m32
"$CC" -c "$DIR/loader/stub.c" -o "$OUT/stub.o" -m32 -ffreestanding -fno-pie -fno-stack-protector -Os
"$CC" -T "$DIR/loader/linker.ld" -o "$OUT/kernel.elf" -ffreestanding -m32 -nostdlib -no-pie \
	-Wl,--build-id=none "$OUT/boot32.o" "$OUT/stub.o" -lgcc

cat > "$OUT/isodir/boot/grub/grub.cfg" <<'CFG'
set timeout=0
menuentry "ravynOS" {
	multiboot2 /boot/kernel.elf
	boot
}
CFG
cp "$OUT/kernel.elf" "$OUT/isodir/boot/kernel.elf"
grub-mkrescue -o "$OUT/test.iso" "$OUT/isodir" --compress=no 2>/dev/null
echo "built $OUT/test.iso"
