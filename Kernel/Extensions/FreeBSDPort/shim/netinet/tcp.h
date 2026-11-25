#ifndef _NETINET_TCP_SHIM_H_
#define _NETINET_TCP_SHIM_H_

#include <sys/types.h>

struct tcphdr {
    uint16_t th_sport;
    uint16_t th_dport;
    uint32_t th_seq;
    uint32_t th_ack;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char th_x2 : 4, th_off : 4;
#else
    unsigned char th_off : 4, th_x2 : 4;
#endif
    unsigned char th_flags;
    uint16_t th_win;
    uint16_t th_sum;
    uint16_t th_urp;
};

#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80

#endif
