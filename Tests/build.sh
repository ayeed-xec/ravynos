#!/bin/sh
set -e
cd "$(dirname "$0")/.."

GCCDIR=$(ls -d /usr/lib/gcc/x86_64-linux-gnu/*/include 2>/dev/null | sort -V | tail -1)
CFLAGS="-I/usr/include/x86_64-linux-gnu/GNUstep -fobjc-runtime=gcc -fconstant-string-class=NSConstantString"
[ -n "$GCCDIR" ] && CFLAGS="$CFLAGS -I$GCCDIR"
LIBS="-lobjc -lgnustep-base -lgnustep-gui"

status=0
for t in stubreplacements cellroundtrip; do
    clang $CFLAGS "Tests/$t.m" -o "/tmp/ravynos-tests-$t" $LIBS
    echo "== $t =="
    "/tmp/ravynos-tests-$t" || status=1
done
exit $status
