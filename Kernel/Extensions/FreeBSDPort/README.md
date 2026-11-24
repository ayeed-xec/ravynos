# FreeBSDPort — FreeBSD driver KPI shim for xnu/IOKit

Purpose: let FreeBSD kernel drivers compile against xnu with a shim instead of
a rewrite. First consumer: virtio-net (`if_vtnet`, pre-iflib FreeBSD 11.x) for
the QEMU guest.

## FreeBSD source manifest (what a vtnet port actually needs)

From FreeBSD 11.x `sys/`:

- `dev/virtio/virtio.c`, `virtqueue.c`, `virtio_bus_if.m`, `virtio_pci.c`
- `dev/virtio/network/if_vtnet.c`, `if_vtnetvar.h`, `virtio_net.h`
- common headers: `dev/virtio/virtio.h`, `virtio_config.h`, `virtio_ring.h`
- support KPI used by the above: `kern/subr_bus_dma.c` semantics, mbuf alloc
  and ring handling, `mutex`, `sx`, `taskqueue`, `callout`, `CONTAINEROF`,
  `bus_space` accessors

Deliberately avoided in step one: `net/iflib.c` (thousands of lines, only
needed for modern em/igb drivers; revisit for e1000).

## Layout

- `shim/FreeBSD/` — shim headers with the FreeBSD names, mapping onto
  Mach/IOKit/libkern primitives (`OSMalloc`, `IOSimpleLock`, `IOMalloc`,
  IOInterruptEventSource workloops, IOBufferMemoryDescriptor).
- Driver sources land next to the shim, one directory per driver
  (`vtnet/`, later `e1000/`), each registered in `Kernel/Extensions/`
  build like the existing families.

## Prerequisite imports (not in this directory)

`IONetworkingFamily` and `IOPCIFamily` from Apple's open-source releases, into
`Kernel/Extensions/`, mirroring how `IOHIDFamily`/`IOPCIFamily` entries sit
there today. Packet path target: shim virtqueue interrupt handler feeds
mbufs into the in-tree xnu `bsd` ifnet interface; TCP/IP needs no changes.
