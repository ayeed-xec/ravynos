#ifndef _NET_IF_ARP_SHIM_H_
#define _NET_IF_ARP_SHIM_H_

#include <sys/types.h>
#include <sys/socket.h>

struct sockaddr_arp {
    unsigned char arp_len;
    sa_family_t arp_family;
    unsigned short arp_hrd;
    unsigned short arp_pro;
    unsigned char arp_hln;
    unsigned char arp_pln;
    unsigned short arp_op;
};

#define ARPHRD_ETHER 1
#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2

#endif
