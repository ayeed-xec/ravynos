#ifndef _MACHINE_BUS_SHIM_H_
#define _MACHINE_BUS_SHIM_H_

#include <sys/types.h>
#include <sys/endian.h>

typedef unsigned long bus_size_t;
typedef uint64_t bus_addr_t;

struct bus_space_tag;
typedef struct bus_space_tag *bus_space_tag_t;
typedef uintptr_t bus_space_handle_t;

int bus_space_map(bus_space_tag_t tag, bus_addr_t addr, bus_size_t size,
    int flags, bus_space_handle_t *handlep);
void bus_space_unmap(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t size);

static inline uint8_t
bus_space_read_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset)
{
    return (*(volatile uint8_t *)(handle + offset));
}

static inline uint16_t
bus_space_read_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset)
{
    return (*(volatile uint16_t *)(handle + offset));
}

static inline uint32_t
bus_space_read_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset)
{
    return (*(volatile uint32_t *)(handle + offset));
}

static inline void
bus_space_write_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, uint8_t value)
{
    *(volatile uint8_t *)(handle + offset) = value;
}

static inline void
bus_space_write_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, uint16_t value)
{
    *(volatile uint16_t *)(handle + offset) = value;
}

static inline void
bus_space_write_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, uint32_t value)
{
    *(volatile uint32_t *)(handle + offset) = value;
}

static inline int
bus_space_read_region_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, uint32_t *dst, bus_size_t count)
{
    bus_size_t i;

    for (i = 0; i < count; i++)
        dst[i] = *(volatile uint32_t *)(handle + offset + i * 4);

    return (0);
}

static inline int
bus_space_write_region_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, const uint32_t *src, bus_size_t count)
{
    bus_size_t i;

    for (i = 0; i < count; i++)
        *(volatile uint32_t *)(handle + offset + i * 4) = src[i];

    return (0);
}

#define BUS_SPACE_BARRIER_READ 1
#define BUS_SPACE_BARRIER_WRITE 2

static inline void
bus_space_barrier(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_size_t offset, bus_size_t length, int flags)
{
    __asm volatile("" ::: "memory");
}

typedef enum {
    BUS_DMA_LOCK,
    BUS_DMA_UNLOCK
} bus_dma_lock_op_t;

typedef int bus_dma_filter_t(void *arg, bus_addr_t paddr);
typedef void bus_dma_lock_t(void *arg, bus_dma_lock_op_t op);

struct bus_dma_tag;
typedef struct bus_dma_tag *bus_dma_tag_t;
struct bus_dmamap;
typedef struct bus_dmamap *bus_dmamap_t;

struct bus_dma_segment {
    bus_addr_t ds_addr;
    bus_size_t ds_len;
};

typedef struct bus_dma_segment bus_dma_segment_t;

typedef void bus_dmamap_callback_t(void *arg, bus_dma_segment_t *segs,
    int nseg, int error);

typedef void bus_dmamap_callback2_t(void *arg, bus_dma_segment_t *segs,
    int nseg, bus_size_t mapsize, int error);

#define BUS_DMA_ALLOCNOW 0x01
#define BUS_DMA_NOWAIT 0x02
#define BUS_DMA_WAITOK 0x04
#define BUS_DMA_ZERO 0x08
#define BUS_DMA_ONESEG 0x10
#define BUS_DMA_COHERENT 0x20

#define BUS_DMA_BUS1 0x40
#define BUS_DMA_BUS2 0x80
#define BUS_DMA_BUS3 0x100
#define BUS_DMA_BUS4 0x200

#define BUS_DMASYNC_PREREAD 1
#define BUS_DMASYNC_POSTREAD 2
#define BUS_DMASYNC_PREWRITE 4
#define BUS_DMASYNC_POSTWRITE 8

#define BUS_SPACE_MAXADDR UINT64_MAX
#define BUS_SPACE_MAXADDR_32BIT UINT32_MAX
#define BUS_SPACE_MAXSIZE UINT64_MAX
#define BUS_SPACE_MAXSIZE_32BIT UINT32_MAX
#define BUS_SPACE_UNRESTRICTED 0

int bus_dma_tag_create(bus_dma_tag_t parent, bus_size_t alignment,
    bus_size_t boundary, bus_addr_t lowaddr, bus_addr_t highaddr,
    bus_dma_filter_t *filter, void *filterarg, bus_size_t maxsize,
    int nsegments, bus_size_t maxsegsz, int flags, bus_dma_lock_t *lockfn,
    void *lockarg, bus_dma_tag_t *dmat);
int bus_dma_tag_destroy(bus_dma_tag_t tag);
int bus_dmamap_create(bus_dma_tag_t tag, int flags, bus_dmamap_t *mapp);
int bus_dmamap_destroy(bus_dma_tag_t tag, bus_dmamap_t map);
int bus_dmamap_load(bus_dma_tag_t tag, bus_dmamap_t map, void *buf,
    bus_size_t buflen, bus_dmamap_callback_t *callback, void *arg,
    int flags);
int bus_dmamap_load_mbuf(bus_dma_tag_t tag, bus_dmamap_t map,
    struct mbuf *mb, bus_dmamap_callback_t *callback, void *arg,
    int flags);
int bus_dmamap_load_mbuf_sg(bus_dma_tag_t tag, bus_dmamap_t map,
    struct mbuf *mb, bus_dma_segment_t *segs, int *nsegs, int flags);
void bus_dmamap_unload(bus_dma_tag_t tag, bus_dmamap_t map);
void bus_dmamap_sync(bus_dma_tag_t tag, bus_dmamap_t map, int op);
int bus_dmamem_alloc(bus_dma_tag_t tag, void **vaddr, int flags,
    bus_dmamap_t *mapp);
void bus_dmamem_free(bus_dma_tag_t tag, void *vaddr, bus_dmamap_t map);

struct resource;
uint8_t bus_read_1(struct resource *r, bus_size_t ofs);
uint16_t bus_read_2(struct resource *r, bus_size_t ofs);
uint32_t bus_read_4(struct resource *r, bus_size_t ofs);
void bus_write_1(struct resource *r, bus_size_t ofs, uint8_t value);
void bus_write_2(struct resource *r, bus_size_t ofs, uint16_t value);
void bus_write_4(struct resource *r, bus_size_t ofs, uint32_t value);
uint64_t vtophys(void *va);

#endif
