#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 The ravynOS Authors
set -uo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

CFLAGS=(-O2 -std=gnu11 -Wall -Wextra -Wshadow -Wconversion -Werror -fuse-ld=lld)
clang "${CFLAGS[@]}" -o "$WORK/rvpatch" "$ROOT/Tools/mig/rvpatch.c" || exit 1
clang "${CFLAGS[@]}" -o "$WORK/mkfat" "$ROOT/Boot/mkfat/mkfat.c" || exit 1
clang "${CFLAGS[@]}" -o "$WORK/elf2pe" "$ROOT/Boot/elf2pe/elf2pe.c" || exit 1

passed=0
failed=0

ok() {
	passed=$((passed + 1))
}

bad() {
	echo "FAIL: $1" >&2
	failed=$((failed + 1))
}

expect_ok() {
	local name=$1
	shift
	if "$@" >"$WORK/out" 2>&1; then ok; else bad "$name: exited non-zero: $(cat "$WORK/out")"; fi
}

expect_fail() {
	local name=$1
	shift
	if "$@" >"$WORK/out" 2>&1; then bad "$name: accepted bad input"; else ok; fi
}

le16() {
	od -An -tu2 -j"$2" -N2 "$1" | tr -d ' '
}

le32() {
	od -An -tu4 -j"$2" -N4 "$1" | tr -d ' '
}

test_rvpatch() {
	local d=$WORK/t-rvpatch
	mkdir -p "$d/tree/src"
	printf 'one\ntwo\nthree\n' >"$d/tree/src/a.txt"
	cat >"$d/good.patch" <<'EOF'
--- a/src/a.txt	2026-01-01
+++ b/src/a.txt	2026-01-01
@@ -1,3 +1,3 @@
 one
-two
+TWO
 three
EOF
	(cd "$d/tree" && expect_ok "rvpatch applies hunk" "$WORK/rvpatch" -p1 "$d/good.patch")
	if [[ "$(cat "$d/tree/src/a.txt")" == $'one\nTWO\nthree' ]]; then ok; else bad "rvpatch output content"; fi

	(cd "$d/tree" && expect_fail "rvpatch rejects reapply" "$WORK/rvpatch" -p1 "$d/good.patch")

	sed 's|a/src/a.txt|a/../escape.txt|; s|b/src/a.txt|b/../escape.txt|' "$d/good.patch" >"$d/dotdot.patch"
	(cd "$d/tree" && expect_fail "rvpatch rejects .." "$WORK/rvpatch" -p1 "$d/dotdot.patch")
	if [[ ! -e "$d/escape.txt" ]]; then ok; else bad "rvpatch wrote outside the tree"; fi

	sed 's|a/src/a.txt|/etc/passwd|; s|b/src/a.txt|/etc/passwd|' "$d/good.patch" >"$d/abs.patch"
	(cd "$d/tree" && expect_fail "rvpatch rejects absolute path" "$WORK/rvpatch" -p0 "$d/abs.patch")

	(cd "$d/tree" && expect_fail "rvpatch rejects -p junk" "$WORK/rvpatch" -p1x "$d/good.patch")
	(cd "$d/tree" && expect_fail "rvpatch rejects negative -p" "$WORK/rvpatch" -p -1 "$d/good.patch")
	(cd "$d/tree" && expect_fail "rvpatch rejects missing patch" "$WORK/rvpatch" -p1 "$d/none.patch")
}

test_mkfat() {
	local d=$WORK/t-mkfat
	mkdir -p "$d"
	head -c 70000 /dev/urandom >"$d/big"
	printf 'kernel' >"$d/small"

	expect_ok "mkfat builds image" "$WORK/mkfat" "$d/esp.img" 16 EFI/BOOT/BOOTRISCV64.EFI "$d/big" kernel "$d/small"
	if [[ $(stat -c %s "$d/esp.img") -eq $((16 * 1024 * 1024)) ]]; then ok; else bad "mkfat image size"; fi
	if [[ "$(le16 "$d/esp.img" 510)" -eq 43605 ]]; then ok; else bad "mkfat boot signature"; fi
	if [[ "$(le16 "$d/esp.img" 512)" -eq 65528 && "$(le16 "$d/esp.img" 514)" -eq 65535 ]]; then ok; else bad "mkfat FAT[0]/FAT[1] media and EOC"; fi

	local fat_sectors
	fat_sectors=$(le16 "$d/esp.img" 22)
	if cmp -s <(dd if="$d/esp.img" bs=512 skip=1 count="$fat_sectors" 2>/dev/null) \
		<(dd if="$d/esp.img" bs=512 skip=$((1 + fat_sectors)) count="$fat_sectors" 2>/dev/null); then ok; else bad "mkfat FAT copies match"; fi

	if LC_ALL=C grep -aq 'BOOTRI~1EFI' "$d/esp.img"; then ok; else bad "mkfat short name for long file"; fi
	if od -An -c -v "$d/esp.img" | tr -d ' \n' | grep -q 'B\\0O\\0O\\0T\\0R\\0'; then ok; else bad "mkfat long name entry"; fi

	expect_ok "mkfat is deterministic" "$WORK/mkfat" "$d/esp2.img" 16 EFI/BOOT/BOOTRISCV64.EFI "$d/big" kernel "$d/small"
	if cmp -s "$d/esp.img" "$d/esp2.img"; then ok; else bad "mkfat output not reproducible"; fi

	expect_fail "mkfat rejects .." "$WORK/mkfat" "$d/x.img" 16 ../evil "$d/small"
	expect_fail "mkfat rejects absolute path" "$WORK/mkfat" "$d/x.img" 16 /evil "$d/small"
	expect_fail "mkfat rejects deep path" "$WORK/mkfat" "$d/x.img" 16 A/B/C/D "$d/small"
	expect_fail "mkfat rejects overlong name" "$WORK/mkfat" "$d/x.img" 16 "EFI/BOOT/A.$(printf 'B%.0s' {1..300})" "$d/small"
	expect_fail "mkfat rejects size 0" "$WORK/mkfat" "$d/x.img" 0 kernel "$d/small"
	expect_fail "mkfat rejects size junk" "$WORK/mkfat" "$d/x.img" 16mb kernel "$d/small"
	expect_fail "mkfat rejects too-small FAT16" "$WORK/mkfat" "$d/x.img" 1 kernel "$d/small"
	expect_fail "mkfat rejects empty file" "$WORK/mkfat" "$d/x.img" 16 kernel /dev/null
	expect_fail "mkfat rejects missing file" "$WORK/mkfat" "$d/x.img" 16 kernel "$d/none"
}

test_elf2pe() {
	local d=$WORK/t-elf2pe
	mkdir -p "$d"
	printf 'void _start(void) { for (;;) { } }\nint data_word = 7;\n' >"$d/k.c"
	local -A machine=([x86_64]=34404 [aarch64]=43620 [riscv64]=20580)
	local arch
	for arch in x86_64 aarch64 riscv64; do
		clang --target="$arch-unknown-none-elf" -ffreestanding -fno-pic -c -O2 -o "$d/k-$arch.o" "$d/k.c" || {
			bad "elf2pe: compile $arch"
			continue
		}
		ld.lld -e _start --image-base=0 -Ttext=0x1000 -o "$d/k-$arch.elf" "$d/k-$arch.o" || {
			bad "elf2pe: link $arch"
			continue
		}
		expect_ok "elf2pe converts $arch" "$WORK/elf2pe" "$d/k-$arch.elf" "$d/k-$arch.efi"
		if [[ "$(head -c 2 "$d/k-$arch.efi")" == MZ ]]; then ok; else bad "elf2pe $arch MZ"; fi
		if [[ "$(le16 "$d/k-$arch.efi" 68)" -eq ${machine[$arch]} ]]; then ok; else bad "elf2pe $arch machine"; fi
		if [[ "$(le16 "$d/k-$arch.efi" 88)" -eq 523 ]]; then ok; else bad "elf2pe $arch PE32+ magic"; fi
		if [[ "$(le16 "$d/k-$arch.efi" 156)" -eq 10 ]]; then ok; else bad "elf2pe $arch EFI subsystem"; fi
		if [[ "$(le32 "$d/k-$arch.efi" 104)" -ne 0 ]]; then ok; else bad "elf2pe $arch entry point"; fi
	done

	expect_fail "elf2pe rejects non-ELF" "$WORK/elf2pe" "$d/k.c" "$d/x.efi"
	expect_fail "elf2pe rejects empty" "$WORK/elf2pe" /dev/null "$d/x.efi"
	expect_fail "elf2pe rejects relocatable" "$WORK/elf2pe" "$d/k-x86_64.o" "$d/x.efi"
	head -c 64 "$d/k-x86_64.elf" >"$d/trunc.elf"
	expect_fail "elf2pe rejects truncated section table" "$WORK/elf2pe" "$d/trunc.elf" "$d/x.efi"
	cp "$d/k-x86_64.elf" "$d/badshoff.elf"
	printf '\xff\xff\xff\xff\xff\xff\xff\x7f' | dd of="$d/badshoff.elf" bs=1 seek=40 conv=notrunc 2>/dev/null
	expect_fail "elf2pe rejects wrapping section offset" "$WORK/elf2pe" "$d/badshoff.elf" "$d/x.efi"
	cp "$d/k-x86_64.elf" "$d/badmach.elf"
	printf '\x03\x00' | dd of="$d/badmach.elf" bs=1 seek=18 conv=notrunc 2>/dev/null
	expect_fail "elf2pe rejects unknown machine" "$WORK/elf2pe" "$d/badmach.elf" "$d/x.efi"
}

test_rvpatch
test_mkfat
test_elf2pe

echo "host tool tests: $passed passed, $failed failed"
[[ $failed -eq 0 ]]
