#ifndef _FREEBSDPORT_PORT_KPI_H_
#define _FREEBSDPORT_PORT_KPI_H_

#include <stdint.h>
#include <stddef.h>
#include <mach/mach_types.h>
#include <libkern/libkern.h>
#include <libkern/OSMalloc.h>
#include <IOKit/IOLocks.h>
#include <IOKit/IOBufferMemoryDescriptor.h>

typedef lck_mtx_t   mtx_t;
typedef lck_grp_t   lock_grp_t;
typedef lck_rw_t    sx_t;
typedef lck_spin_t  spinlock_t;

typedef void (*task_fn_t)(void *context, int pending);

#define MTX_DEF  0
#define MTX_SPIN 1

#define CONTAINEROF(ptr, type, field) \
    ((type)(void *)((char *)(ptr) - __builtin_offsetof(type, field)))

static inline void mtx_init(mtx_t *m, lock_grp_t *g, const char *name, int flags)
{
    (void)name;
    if (flags & MTX_SPIN)
        lck_spin_init((lck_spin_t *)m, g, NULL);
    else
        lck_mtx_init(m, g, NULL);
}

static inline void mtx_lock(mtx_t *m)
{
    lck_mtx_lock(m);
}

static inline void mtx_unlock(mtx_t *m)
{
    lck_mtx_unlock(m);
}

static inline void sx_init(sx_t *s, lock_grp_t *g, const char *name)
{
    (void)name;
    lck_rw_init(s, g, NULL);
}

static inline void sx_slock(sx_t *s)
{
    lck_rw_lock_shared(s);
}

static inline void sx_xlock(sx_t *s)
{
    lck_rw_lock_exclusive(s);
}

static inline void sx_unlock(sx_t *s)
{
    lck_rw_unlock(s);
}

static inline void *contigmalloc_kpi(vm_size_t size, vm_tag_t tag)
{
    return OSMalloc(size, tag);
}

static inline void contigfree_kpi(void *ptr, vm_size_t size, vm_tag_t tag)
{
    OSFree(ptr, size, tag);
}

#endif
