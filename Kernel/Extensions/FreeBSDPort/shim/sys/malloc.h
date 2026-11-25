#ifndef _SYS_MALLOC_SHIM_H_
#define _SYS_MALLOC_SHIM_H_

#include <sys/types.h>

#define M_NOWAIT 0x0001
#define M_WAITOK 0x0002
#define M_ZERO 0x0100
#define M_FIRSTFIT 0x0800
#define M_BESTFIT 0x1000
#define M_USE_RESERVE 0
#define M_USE_INTERRUPT_RESERVE 0

#define MALLOC_DECLARE(type) extern struct malloc_type type[1]

#define MALLOC_DEFINE(type, shortdesc, longdesc) \
    struct malloc_type type[1] = { { shortdesc, NULL } }

struct malloc_type {
    const char *ks_shortdesc;
    void *ks_tag;
};

MALLOC_DECLARE(M_DEVBUF);
MALLOC_DECLARE(M_TEMP);
MALLOC_DECLARE(M_VTNET);
MALLOC_DECLARE(M_VTNET_SHM);
MALLOC_DECLARE(M_MBUF);

void *malloc(unsigned long size, struct malloc_type *type, int flags);
void free(void *addr, struct malloc_type *type);
void *realloc(void *addr, unsigned long size, struct malloc_type *type,
    int flags);
unsigned long malloc_usable_size(void *addr);
int TUNABLE_INT_FETCH(const char *path, int *val);
void *contigmalloc(unsigned long size, struct malloc_type *type, int flags,
    unsigned long low, unsigned long high, unsigned long alignment,
    unsigned long boundary);
void contigfree(void *addr, unsigned long size, struct malloc_type *type);

#endif
