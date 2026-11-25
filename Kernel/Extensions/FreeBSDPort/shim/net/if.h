#ifndef _NET_IF_SHIM_H_
#define _NET_IF_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>

#define IFNAMSIZ 16
#define IF_NAMESIZE IFNAMSIZ
#define IF_MAXADDRLEN 255
#define IF_DUNIT_NONE -1

#define IFF_UP 0x1
#define IFF_BROADCAST 0x2
#define IFF_DEBUG 0x4
#define IFF_LOOPBACK 0x8
#define IFF_POINTOPOINT 0x10
#define IFF_SMART 0x20
#define IFF_DRV_RUNNING 0x40
#define IFF_NOARP 0x80
#define IFF_PROMISC 0x100
#define IFF_ALLMULTI 0x200
#define IFF_DRV_OACTIVE 0x400
#define IFF_SIMPLEX 0x800
#define IFF_LINK0 0x1000
#define IFF_LINK1 0x2000
#define IFF_LINK2 0x4000
#define IFF_ALTPHYS IFF_LINK2
#define IFF_MULTICAST 0x8000
#define IFF_CANTCONFIG 0x10000
#define IFF_PPROMISC 0x20000
#define IFF_MONITOR 0x40000
#define IFF_STATICARP 0x80000
#define IFF_DYING 0x200000
#define IFF_RENAMING 0x400000
#define IFF_NOINET6 0x1000000

#define IFF_CANTCHANGE \
    (IFF_BROADCAST | IFF_DEBUG | IFF_LOOPBACK | IFF_POINTOPOINT | \
    IFF_DRV_RUNNING | IFF_DRV_OACTIVE | IFF_PROMISC | IFF_ALLMULTI | \
    IFF_SMART | IFF_SIMPLEX)

#define IFCAP_RXCSUM 0x00001
#define IFCAP_TXCSUM 0x00002
#define IFCAP_VLAN_MTU 0x00010
#define IFCAP_VLAN_HWTAGGING 0x00020
#define IFCAP_JUMBO_MTU 0x00040
#define IFCAP_POLLING 0x00080
#define IFCAP_VLAN_HWCSUM 0x00100
#define IFCAP_TSO4 0x00200
#define IFCAP_TSO6 0x00400
#define IFCAP_LRO 0x00800
#define IFCAP_WOL_UCAST 0x01000
#define IFCAP_WOL_MCAST 0x02000
#define IFCAP_WOL_MAGIC 0x04000
#define IFCAP_TOE4 0x08000
#define IFCAP_TOE6 0x10000
#define IFCAP_VLAN_HWFILTER 0x20000
#define IFCAP_VLAN_HWTSO 0x40000
#define IFCAP_LINKSTATE 0x80000
#define IFCAP_NETMAP 0x100000
#define IFCAP_RXCSUM_IPV6 0x200000
#define IFCAP_TXCSUM_IPV6 0x400000
#define IFCAP_HWSTATS 0x800000
#define IFCAP_TXRTLMT 0x1000000
#define IFCAP_HWRXTSTMP 0x2000000
#define IFCAP_MEXTPG 0x4000000

#define IFCAP_HWCSUM (IFCAP_RXCSUM | IFCAP_TXCSUM | IFCAP_RXCSUM_IPV6 | \
    IFCAP_TXCSUM_IPV6)
#define IFCAP_WOL (IFCAP_WOL_UCAST | IFCAP_WOL_MCAST | IFCAP_WOL_MAGIC)
#define IFCAP_TSO IFCAP_TSO4
#define IFCAP_TSOv4 IFCAP_TSO4
#define IFCAP_TSOv6 IFCAP_TSO6

typedef enum {
    IFCOUNTER_IPACKETS = 0,
    IFCOUNTER_IERRORS,
    IFCOUNTER_OPACKETS,
    IFCOUNTER_OERRORS,
    IFCOUNTER_COLLISIONS,
    IFCOUNTER_IBYTES,
    IFCOUNTER_OBYTES,
    IFCOUNTER_IMCASTS,
    IFCOUNTER_OMCASTS,
    IFCOUNTER_IQDROPS,
    IFCOUNTER_OQDROPS,
    IFCOUNTER_NOPROTO,
    IFCOUNTERS
} ift_counter;

struct ifnet;

typedef struct ifnet *if_t;

struct ifreq {
    char ifr_name[IFNAMSIZ];
    union {
        struct sockaddr ifru_addr;
        struct sockaddr_dl ifru_dstaddr;
        int ifru_mtu;
        int ifru_flags;
        int ifru_metric;
        int ifru_cap[2];
        u_long ifru_data;
        void *ifru_data_ptr;
    } ifr_ifru;
};

#define ifr_addr ifr_ifru.ifru_addr
#define ifr_dstaddr ifr_ifru.ifru_dstaddr
#define ifr_mtu ifr_ifru.ifru_mtu
#define ifr_flags ifr_ifru.ifru_flags
#define ifr_metric ifr_ifru.ifru_metric
#define ifr_data ifr_ifru.ifru_data
#define ifr_reqcap ifr_ifru.ifru_cap[0]
#define ifr_curcap ifr_ifru.ifru_cap[1]
#define ifr_media ifr_ifru.ifru_data

typedef uint64_t (*if_counter_fn)(if_t, ift_counter);

typedef enum {
    LINK_STATE_UNKNOWN = 0,
    LINK_STATE_DOWN,
    LINK_STATE_UP
} if_link_state;

#endif
