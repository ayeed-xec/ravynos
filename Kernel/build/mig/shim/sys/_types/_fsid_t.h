#ifndef _MIG_SHIM_FSID_T_H
#define _MIG_SHIM_FSID_T_H
typedef struct { int val[2]; } fsid_t_d;
typedef fsid_t_d fsid_t;
typedef unsigned long long __darwin_fsblkcnt_t;
typedef unsigned long long __darwin_fsfilcnt_t;
typedef unsigned int __darwin_id_t;
typedef unsigned int __darwin_useconds_t;
typedef int __darwin_suseconds_t;
#endif
