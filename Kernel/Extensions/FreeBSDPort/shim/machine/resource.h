#ifndef _MACHINE_RESOURCE_SHIM_H_
#define _MACHINE_RESOURCE_SHIM_H_

#include <sys/types.h>

#define SYS_RES_IRQ 1
#define SYS_RES_DRQ 2
#define SYS_RES_MEMORY 3
#define SYS_RES_IOPORT 4

#define BUS_PROBE_DEFAULT (-20)
#define BUS_PROBE_GENERIC 0
#define BUS_PROBE_HOOVER 0
#define BUS_PROBE_NOWILDCARD (-2147483647)

#define VIRTIO_PCI_VRING_ALIGN 4096

extern int bootverbose;

#define FILTER_SCHEDULE_THREAD 1
#define FILTER_STRAY 2
#define FILTER_HANDLED 0

#include <sys/errno.h>

typedef uint64_t rman_res_t;
#define RMAN_MAX UINT64_MAX

#define RF_NONE 0x0000
#define RF_ALLOCATED 0x0001
#define RF_ACTIVE 0x0002
#define RF_SHAREABLE 0x0008
#define RF_FIRSTSHARE 0x0020
#define RF_PREFETCHABLE 0x0100

struct resource;
typedef struct resource *res_t;

#endif
