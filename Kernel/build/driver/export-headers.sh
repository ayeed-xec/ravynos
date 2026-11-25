#!/bin/bash
set -u

DRV="$(cd "$(dirname "$0")" && pwd)"
XNU="$(cd "$DRV/../../xnu" && pwd)"
MIGGEN="$(cd "$DRV/../mig/gen" && pwd)"
OUT="$DRV/export-hdrs"
LOG="$DRV/export-hdrs.log"

STUB="$DRV/.stub-makedefs"
mkdir -p "$STUB/makedefs"
for f in MakeInc.cmd MakeInc.def MakeInc.rule MakeInc.dir MakeInc.top MakeInc.kernel MakeInc.san MakeInc.color; do
	: > "$STUB/makedefs/$f"
done
cat > "$STUB/eval.mk" <<'EOF'
print-vars:
	@echo "ELIST=$(strip ${EXPORT_MI_LIST})"
	@echo "EGEN=$(strip ${EXPORT_MI_GEN_LIST})"
	@echo "EDIR=$(strip ${EXPORT_MI_DIR})"
	@echo "MDLIST=$(strip ${EXPORT_MD_LIST})"
	@echo "MDGEN=$(strip ${EXPORT_MD_GEN_LIST})"
	@echo "MDDIR=$(strip ${EXPORT_MD_DIR})"
EOF

rm -rf "$OUT"
mkdir -p "$OUT"
: > "$LOG"

copied=0
missed=0
collided=0

resolve_src() {
	local dir="$1" name="$2"
	if [ -f "$dir/$name" ]; then
		printf '%s' "$dir/$name"
		return 0
	fi
	local hit
	hit="$(find "$dir" -name "$name" -type f 2>/dev/null | head -1)"
	if [ -n "$hit" ]; then
		printf '%s' "$hit"
		return 0
	fi
	return 1
}

install_flat() {
	local src="$1" name="$2" tag="$3"
	if [ ! -f "$OUT/$name" ]; then
		cp "$src" "$OUT/$name" || return 1
		copied=$((copied + 1))
	elif ! cmp -s "$src" "$OUT/$name"; then
		echo "COLLISION $tag $name (kept first: $(diff -q "$src" "$OUT/$name" >/dev/null 2>&1 || echo same))" >> "$LOG"
		collided=$((collided + 1))
	fi
}

for comp in osfmk bsd libkern pexpert iokit security san EXTERNAL_HEADERS; do
	[ -d "$XNU/$comp" ] || continue
	while IFS= read -r mk; do
		dir="$(dirname "$mk")"
		rel="${mk#"$XNU"/}"
		evalout="$(make -s -C "$dir" -f Makefile -f "$STUB/eval.mk" SRCROOT="$STUB" print-vars 2>>"$LOG")" || {
			echo "MAKEFAIL $rel" >> "$LOG"
			continue
		}
		elist="$(printf '%s\n' "$evalout" | sed -n 's/^ELIST=//p')"
		egen="$(printf '%s\n' "$evalout" | sed -n 's/^EGEN=//p')"
		mdlist="$(printf '%s\n' "$evalout" | sed -n 's/^MDLIST=//p')"
		mdgen="$(printf '%s\n' "$evalout" | sed -n 's/^MDGEN=//p')"
		for list in "$elist" "$egen" "$mdlist" "$mdgen"; do
			for word in $list; do
				case "$word" in
				*.h) ;;
				*) continue ;;
				esac
				if [ -n "$egen" ] && printf '%s\n' "$egen" | grep -qw "$word"; then
					if [ -f "$MIGGEN/$word" ]; then
						install_flat "$MIGGEN/$word" "$word" "mig-gen:$rel" || missed=$((missed + 1))
					else
						echo "MISS-GEN $rel $word" >> "$LOG"
						missed=$((missed + 1))
					fi
					continue
				fi
				if src="$(resolve_src "$dir" "$word")"; then
					install_flat "$src" "$word" "$rel" || missed=$((missed + 1))
				elif [ -f "$MIGGEN/$word" ]; then
					install_flat "$MIGGEN/$word" "$word" "mig-gen:$rel" || missed=$((missed + 1))
				else
					echo "MISS $rel $word" >> "$LOG"
					missed=$((missed + 1))
				fi
			done
		done
	done < <(find "$XNU/$comp" -name Makefile | sort)
done

rm -rf "$STUB"

count="$(ls -1 "$OUT" | wc -l)"
echo "export-headers: $count files into export-hdrs/ ($copied copied, $collided collisions, $missed misses)"
[ "$count" -gt 0 ] || exit 1
