#ifndef _MIG_SHIM_FSID_T_H
#define _MIG_SHIM_FSID_T_H
#define fsid_t linux_fsid_t
#include_next <sys/types.h>
#undef fsid_t
typedef struct { unsigned int val[2]; } fsid_t;
#endif
