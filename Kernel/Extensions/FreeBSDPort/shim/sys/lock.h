#ifndef _SYS_LOCK_SHIM_H_
#define _SYS_LOCK_SHIM_H_

#include <kern/locks.h>

#include <sys/types.h>

#define LK_NOWAIT 1

struct lock_class;
typedef struct lock_class *lock_class_t;

#define MA_OWNED 1
#define MA_NOTOWNED 2
#define MA_RECURSED 3
#define MA_NOTRECURSED 4
#define MA_INITIALIZED 5
#define MA_NOTINITIALIZED 6

#define mtx_assert(m, what) ((void)(m), (void)(what))

#endif
