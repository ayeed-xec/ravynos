#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
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

	cat >"$d/add.patch" <<'EOF'
--- /dev/null
+++ b/src/deep/new.txt
@@ -0,0 +1,2 @@
+alpha
+beta
EOF
	(cd "$d/tree" && expect_ok "rvpatch creates a new file" "$WORK/rvpatch" -p1 "$d/add.patch")
	if [[ "$(cat "$d/tree/src/deep/new.txt" 2>/dev/null)" == $'alpha\nbeta' ]]; then ok; else bad "rvpatch new file content"; fi
	(cd "$d/tree" && expect_fail "rvpatch refuses to overwrite" "$WORK/rvpatch" -p1 "$d/add.patch")
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

test_hash_tools() {
	local d=$WORK/t-hash
	mkdir -p "$d/bin"
	local tool
	while read -r tool; do
		printf 'tool %s\n' "$tool" >"$d/bin/$tool"
	done < <(bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" --list)
	# Files outside the tool list must not enter the hash: an earlier verify
	# hashed `bin/*` and could never match the recorded list.
	printf 'extra\n' >"$d/bin/clang-21"
	printf 'extra\n' >"$d/bin/llvm-objcopy"

	if bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" "$d/bin" >"$d/a" 2>&1; then ok; else bad "hash-tools: hashed a complete bin dir"; fi
	# Same output regardless of the caller's working directory: the recorded
	# file says "clang", never "./clang".
	if ( cd "$d/bin" && bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" . ) >"$d/b" 2>&1; then ok; else bad "hash-tools: ran from another cwd"; fi
	if cmp -s "$d/a" "$d/b"; then ok; else bad "hash-tools: output depends on cwd"; fi
	if grep -q 'clang-21\|llvm-objcopy' "$d/a"; then bad "hash-tools: hashed a tool outside the list"; else ok; fi
	if [[ $(wc -l <"$d/a") -eq $(bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" --list | wc -l) ]]; then ok; else bad "hash-tools: line count differs from the tool list"; fi

	rm -f "$d/bin/ld64.lld"
	expect_fail "hash-tools reports a missing tool" bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" "$d/bin"
	expect_fail "hash-tools rejects a non-directory" bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" "$d/nope"
}

test_xnu_parsers() {
	local out="$WORK/parsers"
	# ADR-17: upstream ships these as Bison output, so they are pruned from the
	# vendored tree and the build regenerates them instead.
	local pruned=(libkern/c++/OSUnserialize.cpp
		iokit/System/OSUnserializeXMLSharedImplementation.h
		libkern/libkern/c++/OSUnserializeXMLSharedImplementation.h)
	local f

	for f in "${pruned[@]}"; do
		if [ -e "$ROOT/Kernel/xnu/$f" ]; then bad "xnu: $f is still in the tree"; else ok; fi
	done

	if bash "$ROOT/Kernel/build/gen-parsers.sh" "$out" >"$WORK/out" 2>&1; then ok; else bad "gen-parsers: exited non-zero: $(cat "$WORK/out")"; fi
	for f in OSUnserialize.cpp OSUnserializeXMLSharedImplementation.h; do
		if [ -s "$out/$f" ] && grep -q YYBYACC "$out/$f"; then ok; else bad "gen-parsers: $f is not byacc output"; fi
	done
	# A reentrant parser keeps its state out of globals; without -P the two
	# grammars would generate a different calling convention.
	if grep -q 'yyparse((void \*)state)' "$out/OSUnserializeXMLSharedImplementation.h"; then ok; else bad "gen-parsers: XML parser lost its state argument"; fi

	expect_fail "gen-parsers needs an output directory" bash "$ROOT/Kernel/build/gen-parsers.sh"
	expect_fail "gen-parsers refuses an unknown extra argument" bash "$ROOT/Kernel/build/gen-parsers.sh" "$out" extra
}

test_doc_paths() {
	# GAP-32: the legacy design doc named paths that were never in the tree. The
	# live documents must keep pointing at files that exist.
	local stale live
	for stale in 'Docs/rfc/' '.mlai/DECISIONS.md' 'Tools/license-audit.py'; do
		if grep -qF -- "$stale" "$ROOT/Docs/DESIGN.md"; then bad "DESIGN.md still names $stale"; else ok; fi
	done
	for live in Docs/design/DECISIONS.md Tools/ci/check-spdx.sh Tools/ci/check-host-tools.sh; do
		if [ -e "$ROOT/$live" ]; then ok; else bad "DESIGN.md points at missing $live"; fi
	done
}

test_stage1_key() {
	local k1 k2 d="$WORK/keytool" copy="$WORK/keycopy" t

	k1=$(bash "$ROOT/Tools/toolchain/llvm/key.sh")
	if [[ $k1 =~ ^[0-9a-f]{64}$ ]]; then ok; else bad "stage1 key is not a sha256: $k1"; fi
	if [ "$(bash "$ROOT/Tools/toolchain/llvm/key.sh")" = "$k1" ]; then ok; else bad "stage1 key is not deterministic"; fi

	# The host tool versions decide what the build produces, so they are keyed.
	mkdir -p "$d/bin"
	printf '#!/bin/sh\necho "cmake version 0.0.0-under-test"\n' >"$d/bin/cmake"
	chmod +x "$d/bin/cmake"
	k2=$(PATH="$d/bin:$PATH" bash "$ROOT/Tools/toolchain/llvm/key.sh")
	if [ "$k1" != "$k2" ]; then ok; else bad "stage1 key ignores the host cmake version"; fi

	# GAP-55: the stage-1 set carries the Mach-O inspection tools stage 2 needs.
	# The list goes into a variable first: piping printf into `grep -q` lets
	# grep exit early, and pipefail then reports the writer's SIGPIPE as a
	# failure even though the tool was found.
	local list
	list=$(bash "$ROOT/Tools/toolchain/llvm/hash-tools.sh" --list)
	for t in llvm-objdump llvm-lipo dsymutil; do
		if grep -qx "$t" <<<"$list"; then ok; else bad "hash-tools: $t is not in the stage-1 list"; fi
	done

	# ... and the content of the scripts that build it.
	cp -a "$ROOT/Tools/toolchain" "$copy"
	printf '\n# a change to a hashed input\n' >>"$copy/llvm/build.sh"
	if [ "$(bash "$ROOT/Tools/toolchain/llvm/key.sh" "$copy")" != "$(bash "$ROOT/Tools/toolchain/llvm/key.sh")" ]; then ok; else bad "stage1 key ignores changes to build.sh"; fi
}

test_buildtools_key() {
	# The build tools and LLVM are separate stage-1 components: a change to one
	# must not move the other's cache key (GAP-55), or a byacc fix would cost a
	# full LLVM rebuild.
	local t="$WORK/bt-tree" bt llvm
	mkdir -p "$t/Tools" "$t/Boot"
	cp -a "$ROOT/Tools/toolchain" "$ROOT/Tools/mig" "$t/Tools/"
	cp -a "$ROOT/Boot/mkfat" "$ROOT/Boot/elf2pe" "$t/Boot/"
	cp "$ROOT/VENDORS" "$t/"

	bt=$(bash "$t/Tools/toolchain/buildtools/key.sh")
	llvm=$(bash "$ROOT/Tools/toolchain/llvm/key.sh" "$t/Tools/toolchain")
	if [[ $bt =~ ^[0-9a-f]{64}$ ]]; then ok; else bad "buildtools key is not a sha256: $bt"; fi
	if [ "$bt" = "$(bash "$ROOT/Tools/toolchain/buildtools/key.sh")" ]; then ok; else bad "buildtools key depends on the checkout path"; fi

	printf '\n# a change to an LLVM input\n' >>"$t/Tools/toolchain/llvm/build.sh"
	if [ "$(bash "$t/Tools/toolchain/buildtools/key.sh")" = "$bt" ]; then ok; else bad "an LLVM change moved the buildtools key"; fi
	if [ "$(bash "$ROOT/Tools/toolchain/llvm/key.sh" "$t/Tools/toolchain")" != "$llvm" ]; then ok; else bad "llvm key ignores llvm/build.sh"; fi
	llvm=$(bash "$ROOT/Tools/toolchain/llvm/key.sh" "$t/Tools/toolchain")

	local input
	for input in Tools/toolchain/buildtools/build.sh Tools/mig/patches/series Boot/mkfat/mkfat.c VENDORS; do
		if [ "$input" = VENDORS ]; then
			sed -i 's/"t20260126"/"t20251214"/' "$t/VENDORS"
		else
			printf '\n' >>"$t/$input"
		fi
		if [ "$(bash "$t/Tools/toolchain/buildtools/key.sh")" != "$bt" ]; then ok; else bad "buildtools key ignores $input"; fi
		bt=$(bash "$t/Tools/toolchain/buildtools/key.sh")
		if [ "$(bash "$ROOT/Tools/toolchain/llvm/key.sh" "$t/Tools/toolchain")" = "$llvm" ]; then ok; else bad "a change to $input moved the llvm key"; fi
	done
	# Only the byacc and unifdef pins are keyed, not the rest of VENDORS.
	printf '\n# unrelated\n' >>"$t/VENDORS"
	if [ "$(bash "$t/Tools/toolchain/buildtools/key.sh")" = "$bt" ]; then ok; else bad "an unrelated VENDORS edit moved the buildtools key"; fi

	expect_ok "vendor-pin reads a fetched pin" bash "$ROOT/Tools/toolchain/buildtools/vendor-pin.sh" byacc
	if [[ $(wc -l <"$WORK/out") -eq 3 ]] && grep -qx '[0-9a-f]\{40\}' "$WORK/out"; then ok; else bad "vendor-pin output is not upstream, tag, commit"; fi
	expect_fail "vendor-pin refuses a committed tree" bash "$ROOT/Tools/toolchain/buildtools/vendor-pin.sh" xnu
	expect_fail "vendor-pin refuses an unknown name" bash "$ROOT/Tools/toolchain/buildtools/vendor-pin.sh" no-such-tree
	expect_fail "buildtools --verify refuses a cold cache" bash "$t/Tools/toolchain/buildtools.sh" --verify
	if grep -q "not built" "$WORK/out"; then ok; else bad "buildtools --verify did not say the cache is cold"; fi
	expect_fail "buildtools rejects an unknown flag" bash "$ROOT/Tools/toolchain/buildtools.sh" --fast
}

test_commit_subjects() {
	# GAP-59: subjects stay short and lowercase-scoped. The checker reads the
	# repository it lives in, so run a copy inside a scratch repository.
	local d="$WORK/commitrepo"
	rm -rf "$d"
	mkdir -p "$d/Tools/ci"
	cp "$ROOT/Tools/ci/check-commit-subjects.sh" "$d/Tools/ci/"
	git -C "$d" init -q
	git -C "$d" config user.email test@example.invalid
	git -C "$d" config user.name test
	printf 'a\n' >"$d/a"
	git -C "$d" add a
	git -C "$d" commit -qm "rv: a good subject"

	if bash "$d/Tools/ci/check-commit-subjects.sh" 'HEAD^..HEAD' >/dev/null 2>&1; then ok; else bad "commit-subjects rejects a good subject"; fi

	printf 'b\n' >"$d/b"
	git -C "$d" add b
	git -C "$d" commit -qm "Docs: mixed case prefix"
	expect_fail "commit-subjects rejects an uppercase scope" bash "$d/Tools/ci/check-commit-subjects.sh" 'HEAD^..HEAD'

	printf 'c\n' >"$d/c"
	git -C "$d" add c
	git -C "$d" commit -qm "rv: $(printf 'x%.0s' $(seq 1 80))"
	expect_fail "commit-subjects rejects an over-long subject" bash "$d/Tools/ci/check-commit-subjects.sh" 'HEAD^..HEAD'

	# ADR-32: commits at or before the accepted-history baseline are skipped,
	# so a whole-history range does not fail on decisions already taken.
	local baseline
	baseline=$(git -C "$d" rev-parse HEAD)
	if COMMIT_RULE_BASELINE=$baseline bash "$d/Tools/ci/check-commit-subjects.sh" HEAD >/dev/null 2>&1; then ok; else bad "commit-subjects ignores the accepted-history baseline"; fi
}

test_rvpatch
test_mkfat
test_elf2pe
test_hash_tools
test_xnu_parsers
test_doc_paths
test_stage1_key
test_buildtools_key
test_commit_subjects

echo "host tool tests: $passed passed, $failed failed"
[[ $failed -eq 0 ]]
