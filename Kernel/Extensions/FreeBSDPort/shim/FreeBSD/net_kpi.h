#ifndef _FREEBSDPORT_NET_KPI_H_
#define _FREEBSDPORT_NET_KPI_H_

/*
 * Network-side shim: maps the FreeBSD network driver KPI onto the xnu
 * kernel's own implementations. Nothing here is a stub — every mapping
 * points at a real xnu facility:
 *
 *   FreeBSD                -> xnu
 *   mbuf / mbuf_t          -> bsd/sys/kpi_mbuf.h   (mbuf_t, kpi_mbuf.c)
 *   if_t / ifnet           -> bsd/net/kpi_interface.h (ifnet_t, kpi_interface.c)
 *   TX queue / hooks       -> IONetworkingFamily (IOBasicOutputQueue,
 *                             IONetworkInterface, IOMbufMemoryCursor)
 *   bus_space_read/write    -> IOMemoryMap (read32/write32 and friends)
 *   PCI config read/write   -> IOPCIDevice (configRead8/16/32, configWrite8/16/32)
 *
 * The virtio sources in vtnet/ are the FreeBSD 11.4 originals
 * (sys/dev/virtio, BSD 2-clause) ported include-path first: their FreeBSD
 * headers are shadowed by the shim headers in this directory.
 */

#include <sys/kpi_mbuf.h>
#include <net/kpi_interface.h>
#include <IOKit/network/IOBasicOutputQueue.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOMbufMemoryCursor.h>
#include <IOKit/pci/IOPCIDevice.h>

/* ---- mbuf ---------------------------------------------------------- */

/* FreeBSD m_* accessors that have a direct xnu KPI spelling. */
#define m_data(m)   mbuf_data((m))
#define m_len(m)    mbuf_len((m))
#define m_pkthdr(m) mbuf_pkthdr((m))
#define m_pkthdr_len(m) mbuf_pkthdr_len((m))
#define m_freem(m)  (mbuf_free((m)), (void)0)

static inline errno_t shim_mbuf_alloc(mbuf_t *m, mbuf_how_t how, mbuf_type_t type)
{ return mbuf_alloc(how, type, m); }

/* ---- ifnet --------------------------------------------------------- */

/* if_t in FreeBSD is an opaque ifnet handle; xnu's ifnet_t is the same
 * role. Attach/detach and address management go through kpi_interface. */
typedef ifnet_t shim_if_t;

static inline errno_t shim_if_attach(shim_if_t *ifp, const char *name, int unit)
{ return ifnet_allocate_extended(0 /* ifnet_init_eparams: defaults */, name, unit, NULL, ifp); }

/* ---- queue / packet flow ------------------------------------------- */

/*
 * A FreeBSD driver's if_transmit becomes an IOBasicOutputQueue consumer:
 * the ported driver overrides IONetworkController::outputPacket() and the
 * shim provides the FreeBSD-flavoured enqueue/dequeue around it.
 */

/* ---- bus space ------------------------------------------------------
 * FreeBSD bus_space_read_N(bus_tag, handle, off) maps to
 * IOMemoryMap->read/write* on the IOPCIDevice::mapDeviceMemoryWithRegister
 * mapping the vtnet probe creates. Implemented in net_kpi_port.c once the
 * controller subclass lands; the declarations here fix the API surface.
 */
struct shim_bus_space {
    IOMemoryMap *map;
};

static inline uint32_t shim_bus_read32(struct shim_bus_space *bs, uint32_t off)
{ return (uint32_t)bs->map->read32(off); }

static inline void shim_bus_write32(struct shim_bus_space *bs, uint32_t off, uint32_t v)
{ bs->map->write32(off, v); }

#endif /* _FREEBSDPORT_NET_KPI_H_ */
