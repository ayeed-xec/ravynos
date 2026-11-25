#ifndef _MIG_SHIM_U_INT64_T_H
#define _MIG_SHIM_U_INT64_T_H
#if defined(__linux__)
typedef __UINT64_TYPE__ u_int64_t;
#else
typedef unsigned long long u_int64_t;
#endif
#endif
