#ifndef _SYS_SGLIST_SHIM_H_
#define _SYS_SGLIST_SHIM_H_

#include <sys/types.h>
#include <machine/bus.h>
#include <sys/queue.h>

struct mbuf;

struct sglist_seg {
    uint64_t ss_paddr;
    bus_size_t ss_len;
};

struct sglist {
    struct sglist_seg *sg_segs;
    uint16_t sg_nseg;
    uint16_t sg_maxseg;
    int sg_refcnt;
};

int sglist_init(struct sglist *sg, int nsegs, struct sglist_seg *segs);
struct sglist *sglist_alloc(int nsegs, int mflags);
void sglist_free(struct sglist *sg);
int sglist_append(struct sglist *sg, void *buf, size_t len);
int sglist_append_mbuf(struct sglist *sg, struct mbuf *m);
int sglist_count(void *buf, size_t len);
size_t sglist_length(struct sglist *sg);
int sglist_split(struct sglist *original, struct sglist *head, size_t length);
int sglist_reset(struct sglist *sg);
int sglist_append_sglist(struct sglist *sg, struct sglist *src, size_t offset,
    size_t length);

#endif
