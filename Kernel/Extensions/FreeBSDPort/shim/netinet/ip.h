#ifndef _NETINET_IP_SHIM_H_
#define _NETINET_IP_SHIM_H_

#include <sys/types.h>
#include <netinet/in.h>

struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char ip_hl : 4, ip_v : 4;
#else
    unsigned char ip_v : 4, ip_hl : 4;
#endif
    unsigned char ip_tos;
    unsigned short ip_len;
    unsigned short ip_id;
    unsigned short ip_off;
    unsigned char ip_ttl;
    unsigned char ip_p;
    unsigned short ip_sum;
    struct in_addr ip_src, ip_dst;
};

#define IP_MAXPACKET 65535
#define IP_HDR_ALIGNED_P(a) 1

#endif
