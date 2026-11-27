# XNU-first plan: FreeBSD drivers, performance, UI stack, Claude Desktop POC

> Historical plan from before the current design. Superseded by [design/SYSTEM-DESIGN.md](design/SYSTEM-DESIGN.md); its status claims are not current.

Decision (2026-11-24): xnu is the kernel of record. The FreeBSD ISO path stays as a
hosting convenience, not a second OS. All new low-level work targets
`Kernel/xnu` and kexts under `Kernel/Extensions/`.

## Workstream 1 — xnu extended with FreeBSD drivers

Current state (verified): `Kernel/xnu/iokit` has only NVRAM/power/platform
families. There is no IONetworkingFamily, no IOPCIFamily import for networking,
and no NIC driver anywhere in the tree. The xnu `bsd/` subtree already carries
the full TCP/IP stack (netinet, ifnet), so the missing layer is exactly:
networking kext families + a driver.

Order of work:

1. Import `IONetworkingFamily` and `IOPCIFamily` (Apple open-source releases)
   under `Kernel/Extensions/` next to the existing families.
2. Build `Kernel/Extensions/FreeBSDPort/`: shim headers mapping the FreeBSD
   kernel KPI (malloc, mutex/sx, bus_dma, mbuf, taskqueue, CONTAINEROF) onto
   Mach/IOKit primitives. Drivers compile against the shim, unmodified where
   possible.
3. First driver: virtio-net, because the Phase 1 QEMU guest (`-machine q35`,
   virtio) is the only hardware we can test on. Port the pre-iflib
   (FreeBSD 11.x) `if_vtnet` to avoid porting the whole iflib framework in
   step one; consider iflib later for e1000/igb.
4. Deliver packets into the existing xnu `bsd` ifnet path; DHCP + TCP then
   work with no userland changes.

Status (2026-11-24): steps 1-3 imports done. IONetworkingFamily-188 imported
from opensource.apple.com into `Kernel/Extensions/IONetworkingFamily`
(IOPCIFamily was already in-tree from PureDarwin); provenance recorded in
`Kernel/_PROVENANCE`. FreeBSDPort grew `shim/FreeBSD/net_kpi.h`: FreeBSD
mbuf/if_t map onto xnu's own real KPIs (`bsd/sys/kpi_mbuf.h`,
`bsd/net/kpi_interface.h`), bus_space onto IOMemoryMap, PCI config onto
IOPCIDevice — no stubs. FreeBSD 11.4 virtio sources imported verbatim into
`FreeBSDPort/vtnet/` (virtio.c, virtqueue.c, virtio_pci.c, if_vtnet.c
[3968 lines], headers, bus interfaces). Shim include tree under
`FreeBSDPort/shim/IOKit/network/` forwards `<IOKit/network/...>` to the
imported sources. Compile status: includes resolve deep into the xnu tree;
the remaining blocker for host syntax checks is `mach/clock.h`, a
mig-generated header (from `osfmk/mach/clock.defs`) — no `mig` on this host.
Next: run the xnu headers build to generate mach headers, then
clang-syntax the shim + vtnet port, then wire the controller subclass.

## Workstream 2 — xnu performance

- Host: this machine has no `/dev/kvm`, so QEMU runs TCG. Levers available:
  `-accel tcg,thread=multi` (MTTCG, supported for x86-on-x86) instead of the
  current `thread=single`, and `-smp 2`. Measure boot wall time before/after
  with the existing boot-sweep recipe.
- Kernel build: the Kernel Makefile honours `KERNEL_CONFIGS`; build
  DEVELOPMENT/RELEASE (not DEBUG) for the guest image; `BUILD_LTO=0` is set
  today — LTO is a later experiment, measured, not assumed.

## Workstream 3 — UI stack (Phase 2, kernel-agnostic)

Unchanged by the kernel decision. Current state: `apps/first-window` builds
and renders against GNUstep; the ravynOS-native path is the
AppKit→WindowServer.backend IPC glue (commit 3d7841f451), compile-verified
only. Next: run that backend against the real WindowServer contract in the
guest, then make `first-window` render through it.

## Workstream 4 — Claude Desktop POC (the target app)

Claude Desktop on macOS is an Electron app (Chromium + Node.js, multi-process,
GPU via ANGLE, heavy AppKit/CoreFoundation/IOKit-userland usage). That makes it
the hardest class of POC, so it is gated, not attempted directly:

| Gate | Done when |
|---|---|
| G0 | static Mach-O runs in guest (DONE, Phase 1) |
| G1 | WindowServer shows first-window with a button (Phase 2) |
| G2 | dyld loads dynamic frameworks in the guest |
| G3 | a trivial dynamically-linked AppKit app runs against our frameworks |
| G4 | Mach bootstrap/launchd + IOKit userland services Chromium needs exist |
| G5 | Chromium/Electron renderer launches headless in the guest |
| G6 | Claude Desktop .app launches and shows the login screen |

G6 without G1-G5 is not a shortcut; each gate de-risks one Electron
dependency layer (window server, dyld, ObjC ABI, system services, Chromium).
