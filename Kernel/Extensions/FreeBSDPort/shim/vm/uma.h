#ifndef _VM_UMA_SHIM_H_
#define _VM_UMA_SHIM_H_

#include <sys/types.h>

typedef struct uma_zone *uma_zone_t;
typedef struct uma_zone *uma_zone_ext_t;

#define UMA_ALIGN_PTR 8
#define UMA_ALIGN_CACHE 64
#define UMA_ZONE_CACHE 0
#define UMA_ZONE_NOFREE 0
#define UMA_ZONE_NODUMP 0

typedef int (*uma_ctor_t)(void *mem, int size, void *udata, int flags);
typedef void (*uma_dtor_t)(void *mem, int size, void *udata);
typedef int (*uma_init_t)(void *mem, int size, int flags);
typedef void (*uma_fini_t)(void *mem, int size);
typedef void *(*uma_alloc_t)(uma_zone_t zone, int size);
typedef void (*uma_free_t)(uma_zone_t zone, void *mem, int size);

uma_zone_t uma_zcreate(const char *name, size_t size, uma_ctor_t ctor,
    uma_dtor_t dtor, uma_init_t zinit, uma_fini_t zfini, int align,
    uint32_t flags);
void uma_zdestroy(uma_zone_t zone);
void *uma_zalloc(uma_zone_t zone, int flags);
void *uma_zalloc_arg(uma_zone_t zone, void *udata, int flags);
void uma_zfree(uma_zone_t zone, void *item);
void uma_zfree_arg(uma_zone_t zone, void *item, void *udata);
uint64_t uma_zone_get_cur(uma_zone_t zone);

#endif
