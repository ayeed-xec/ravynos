#ifndef _MIG_SHIM_INT64_T_H
#define _MIG_SHIM_INT64_T_H
#if defined(__linux__)
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ u_int64_t;
#else
typedef long long int64_t;
typedef unsigned long long u_int64_t;
typedef long long __int64_t;
typedef unsigned long long __uint64_t;
#endif
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
#endif
