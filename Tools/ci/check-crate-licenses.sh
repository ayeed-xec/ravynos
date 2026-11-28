#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 The ravynOS Authors
#
# Every crate a Cargo.lock resolves must be under a licence ADR-24 accepts
# (GAP-09). `cargo tree --locked` is the resolution the build itself uses, so a
# lockfile that grew a copyleft dependency fails here before the image ships it.
#
# CRATE_LICENSES_FILE replays a saved `cargo tree --format '{p}|{l}'` listing;
# the planted test uses it to prove the check can fail on an offline runner.
set -uo pipefail

root=$(CDPATH="" cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$root" || exit 1

# ADR-24: the permissive licences, plus the ones that ADR text adds.
allow='0BSD APSL-2.0 BSD BSD-2-Clause BSD-3-Clause MIT Apache-2.0 Zlib ISC CC0-1.0 Unlicense LicenseRef-PublicDomain BSL-1.0 FTL libpng-2.0 X11 bzip2-1.0.6 IJG libtiff ZSH curl Less SCOWL OpenSSL Unicode-3.0'

allowed() {
	case " $allow " in
	*" $1 "*) return 0 ;;
	esac
	return 1
}

# verdict <package> <licence expression>
verdict() {
	local pkg=$1 expr=$2 term
	expr=${expr%% WITH *}
	expr=${expr//\// }
	expr=${expr//(/ }
	expr=${expr//)/ }
	expr=${expr// OR / }
	expr=${expr// AND / }
	builtin read -r -a terms <<<"$expr"
	for term in "${terms[@]}"; do
		[ -n "$term" ] || continue
		if ! allowed "$term"; then
			echo "error: $pkg is licensed '$expr', and $term is not on the ADR-24 list" >&2
			return 1
		fi
	done
	return 0
}

status=0
checked=0

if [ -n "${CRATE_LICENSES_FILE:-}" ]; then
	listing=$(cat "$CRATE_LICENSES_FILE")
else
	locks=$(git ls-files -- 'Cargo.lock' '*/Cargo.lock')
	if [ -z "$locks" ]; then
		echo "crate licence gate: no Cargo.lock tracked"
		exit 0
	fi
	listing=""
	while IFS= read -r lock; do
		manifest=${lock%Cargo.lock}Cargo.toml
		out=$(cargo tree --locked --prefix none --edges normal,build --manifest-path "$manifest" --format '{p}|{l}' 2>&1) || {
			printf '%s\n' "$out" >&2
			echo "error: cargo tree failed for $manifest; the gate cannot see licences" >&2
			status=1
			continue
		}
		listing+="${out}"$'\n'
	done <<<"$locks"
fi

while IFS= read -r line; do
	pkg=${line%%|*}
	lic=${line##*|}
	case "$pkg" in
	*'('*')') pkg=${pkg% (*} ;;
	esac
	[ -n "$lic" ] || continue # a workspace crate carries no third-party licence
	checked=$((checked + 1))
	verdict "$pkg" "$lic" || status=1
done <<<"$listing"

if [ "$status" -ne 0 ]; then
	echo "crate licence gate: FAILED" >&2
	exit 1
fi
echo "crate licence gate OK: $checked crates, all under an ADR-24 licence"
