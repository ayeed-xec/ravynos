#ifndef _NET_IF_ADDR_SHIM_H_
#define _NET_IF_ADDR_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <net/if_var.h>

struct ifaddr {
    TAILQ_ENTRY(ifaddr) ifa_link;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_dstaddr;
    struct sockaddr *ifa_netmask;
    struct ifnet *ifa_ifp;
    int ifa_flags;
    int ifa_refcnt;
};

struct ifmultiaddr {
    TAILQ_ENTRY(ifmultiaddr) ifma_link;
    struct sockaddr *ifma_addr;
    struct sockaddr *ifma_lladdr;
    struct ifnet *ifma_ifp;
    int ifma_refcount;
};

int ifa_add_ref(struct ifaddr *ifa);
int ifa_del_ref(struct ifaddr *ifa);
void ifa_free(struct ifaddr *ifa);
struct ifaddr *ifaddr_find(struct ifnet *ifp, struct sockaddr *sa);

#endif
