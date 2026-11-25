#ifndef _NET_IF_VLAN_VAR_SHIM_H_
#define _NET_IF_VLAN_VAR_SHIM_H_

#include <sys/types.h>

struct ifnet;

#define VLAN_CAPABILITIES(ifp) shim_vlan_capab((ifp))

void shim_vlan_capab(struct ifnet *ifp);

#endif
