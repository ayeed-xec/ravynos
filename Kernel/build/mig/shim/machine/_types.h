#ifndef _MIG_SHIM_MACHINE__TYPES_H
#define _MIG_SHIM_MACHINE__TYPES_H
#include <stdint.h>
typedef signed char __darwin_int8_t;
typedef unsigned char __darwin_uint8_t;
typedef short __darwin_int16_t;
typedef unsigned short __darwin_uint16_t;
typedef int __darwin_int32_t;
typedef unsigned int __darwin_uint32_t;
typedef long long __darwin_int64_t;
typedef unsigned long long __darwin_uint64_t;
typedef long __darwin_intptr_t;
typedef unsigned long __darwin_uintptr_t;
typedef long __darwin_ptrdiff_t;
typedef long __darwin_size_t;
typedef long __darwin_ssize_t;
typedef long __darwin_off_t;

typedef unsigned long __darwin_socklen_t;
typedef unsigned int __darwin_uid_t;
typedef unsigned int __darwin_gid_t;
typedef unsigned int __darwin_id_t;
typedef unsigned int __darwin_useconds_t;
typedef int __darwin_suseconds_t;
typedef long long __darwin_off64_t;
typedef unsigned long long __darwin_ino_t;
typedef unsigned long long __darwin_ino64_t;
typedef long __darwin_time_t;
typedef unsigned long __darwin_natural_t;
typedef unsigned long long __darwin_fsblkcnt_t;
typedef unsigned long long __darwin_fsfilcnt_t;
#endif
