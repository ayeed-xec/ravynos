#ifndef _SYS_PARAM_SHIM_H_
#define _SYS_PARAM_SHIM_H_

#include <sys/types.h>

#define BSD 199506

#include <machine/vmparam.h>

#define NBBY 8
#define HZ 100

extern int hz;
extern int tick;

#define MAXCOMLEN 16
#define MAXHOSTNAMELEN 256

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE (1 << PAGE_SHIFT)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (PAGE_SIZE - 1)
#endif

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 128
#endif

#define MSIZE 256
#define MCLBYTES 2048
#define MJUMPAGESIZE PAGE_SIZE
#define MJUM9BYTES 9216
#define MJUM16BYTES 16384

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#define rounddown(x, y) (((x) / (y)) * (y))
#define rounddown2(x, y) ((x) & ~((y) - 1))
#define roundup2(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
#define powerof2(x) ((((x) - 1) & (x)) == 0)
#define trunc_page(x) ((x) & ~PAGE_MASK)
#define round_page(x) (((x) + PAGE_MASK) & ~PAGE_MASK)

#endif
