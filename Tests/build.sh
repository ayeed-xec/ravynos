#!/bin/sh
set -e
cd "$(dirname "$0")/.."

GCCDIR=$(ls -d /usr/lib/gcc/x86_64-linux-gnu/*/include 2>/dev/null | sort -V | tail -1)
CFLAGS="-I/usr/include/x86_64-linux-gnu/GNUstep -fobjc-runtime=gcc -fconstant-string-class=NSConstantString"
[ -n "$GCCDIR" ] && CFLAGS="$CFLAGS -I$GCCDIR"
LIBS="-lobjc -lgnustep-base -lgnustep-gui"

status=0
held=""
for t in stubreplacements cellroundtrip foundation-cli; do
    clang $CFLAGS "Tests/$t.m" -o "/tmp/ravynos-tests-$t" $LIBS
    if "/tmp/ravynos-tests-$t" > "/tmp/ravynos-tests-$t.out" 2>&1; then
        held="$held /tmp/ravynos-tests-$t.out"
    else
        status=1
        echo "== $t =="
        cat "/tmp/ravynos-tests-$t.out"
    fi
done
if [ $status -eq 0 ]; then
    for out in $held; do
        echo "== $(basename "$out" .out | sed 's/ravynos-tests-//') =="
        cat "$out"
    done
fi
exit $status
