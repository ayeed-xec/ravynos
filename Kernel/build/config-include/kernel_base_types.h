#ifndef _SHIM_KERNEL_BASE_TYPES_H
#define _SHIM_KERNEL_BASE_TYPES_H

/*
 * Stand-in for the BSD sys/types.h base types that xnu kernel headers
 * expect from the SDK's clang modules (USE_CLANG_TYPES) in the real
 * build. Clean-room; matches the bsd/sys/_types/_u_int.h values.
 */

#include <sys/cdefs.h>
#include <sys/_types/_u_int.h>

#ifndef _SHIM_HAS_UINT_T
#define _SHIM_HAS_UINT_T
typedef unsigned int    uint_t;
typedef volatile unsigned int   vuint_t;
typedef unsigned int    u_int;
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned long   u_long;
#endif

#endif
