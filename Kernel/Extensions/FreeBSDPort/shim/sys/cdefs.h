#ifndef _SYS_CDEFS_SHIM_H_
#define _SYS_CDEFS_SHIM_H_

#include_next <sys/cdefs.h>

#ifndef __FBSDID
#define __FBSDID(s) typedef char __fbSDID_shim_t
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __section
#define __section(x) __attribute__((__section__(x)))
#endif

#ifndef __weak_symbol
#define __weak_symbol __attribute__((__weak__))
#endif

#ifndef __strong_reference
#define __strong_reference(sym, aliassym)
#endif

#ifndef __printflike
#define __printflike(fmtarg, firstvararg) \
    __attribute__((__format__(__printf__, fmtarg, firstvararg)))
#endif

#ifndef __scanflike
#define __scanflike(fmtarg, firstvararg) \
    __attribute__((__format__(__scanf__, fmtarg, firstvararg)))
#endif

#ifndef __offsetof
#define __offsetof(type, field) __builtin_offsetof(type, field)
#endif

#ifndef __rangeof
#define __rangeof(type, start, end) \
    (__offsetof(type, end) - __offsetof(type, start))
#endif

#ifndef CTASSERT
#define _CTASSERT(x, y) typedef char __assert_ ## y[(x) ? 1 : -1]
#define CTASSERT(x) _CTASSERT(x, __LINE__)
#endif

#ifndef __DECONST
#define __DECONST(type, var) ((type)(__uintptr_t)(const void *)(var))
#endif

#ifndef __CONCAT
#define __CONCAT1(a, b) a ## b
#define __CONCAT(a, b) __CONCAT1(a, b)
#endif

#endif
