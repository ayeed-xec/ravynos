#ifndef _SYS_MUTEX_SHIM_H_
#define _SYS_MUTEX_SHIM_H_

#include <sys/lock.h>
#include <IOKit/IOLocks.h>

#define MTX_DEF 0
#define MTX_SPIN 1
#define MTX_QUIET 2

struct mtx {
    IOLock *m_lock;
    const char *m_name;
};

void mtx_init(struct mtx *m, const char *name, const char *type, int opts);
void mtx_destroy(struct mtx *m);
void mtx_lock(struct mtx *m);
void mtx_unlock(struct mtx *m);
int mtx_trylock(struct mtx *m);
int mtx_initialized(struct mtx *m);

#endif
