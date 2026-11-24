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

## Status

- `shim/FreeBSD/net_kpi.h` — network KPI mappings: FreeBSD mbuf/if_t map
  onto xnu's own `kpi_mbuf.h`/`kpi_interface.h` (real implementations, no
  stubs); bus_space accessors map onto `IOMemoryMap` reads/writes; PCI
  config accessors map onto `IOPCIDevice`.
- `vtnet/` — FreeBSD 11.4 originals imported verbatim (BSD 2-clause):
  `virtio.c`, `virtqueue.c`, `virtio_pci.c`, `if_vtnet.c` and their
  headers (3968 lines in `if_vtnet.c` alone). Porting work = shadowing
  their includes with the shim and registering the controller subclass;
  packet path is shim virtqueue interrupt handler feeding mbufs through
  IONetworkingFamily into the in-tree xnu `bsd` ifnet interface.

## Prerequisite imports (now done)

- `Kernel/Extensions/IONetworkingFamily` — imported from the Apple
  opensource.apple.com tarball `IONetworkingFamily-188` (APSL-2.0); see
  `Kernel/_PROVENANCE`.
- `Kernel/Extensions/IOPCIFamily` — already in-tree from the PureDarwin
  import.

