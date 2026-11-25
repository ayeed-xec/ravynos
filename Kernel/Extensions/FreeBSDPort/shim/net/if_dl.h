#ifndef _NET_IF_DL_SHIM_H_
#define _NET_IF_DL_SHIM_H_

#include <sys/socket.h>
#include <net/if.h>

void link_init_sdl(if_t ifp, struct sockaddr_dl *sdl, u_char iftype);
struct sockaddr_dl *if_lladdr(if_t ifp);

#endif
