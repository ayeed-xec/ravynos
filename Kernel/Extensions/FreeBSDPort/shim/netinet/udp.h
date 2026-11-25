#ifndef _NETINET_UDP_SHIM_H_
#define _NETINET_UDP_SHIM_H_

#include <sys/types.h>

struct udphdr {
    uint16_t uh_sport;
    uint16_t uh_dport;
    uint16_t uh_ulen;
    uint16_t uh_sum;
};

#endif
