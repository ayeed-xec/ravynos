#ifndef _SYS_BUF_RING_SHIM_H_
#define _SYS_BUF_RING_SHIM_H_

#include <sys/types.h>
#include <sys/mutex.h>

struct buf_ring;

struct buf_ring *buf_ring_alloc(int count, struct malloc_type *type,
    int flags, struct mtx *lock);
void buf_ring_free(struct buf_ring *br, struct malloc_type *type);
int buf_ring_enqueue(struct buf_ring *br, void *buf);
void *buf_ring_dequeue_sc(struct buf_ring *br);
void *buf_ring_peek(struct buf_ring *br);
void *buf_ring_peek_sc(struct buf_ring *br);
void buf_ring_putback_sc(struct buf_ring *br, void *new_buf);
void *buf_ring_advance_sc(struct buf_ring *br);
int buf_ring_full(struct buf_ring *br);
int buf_ring_empty(struct buf_ring *br);
int buf_ring_count(struct buf_ring *br);

#endif
