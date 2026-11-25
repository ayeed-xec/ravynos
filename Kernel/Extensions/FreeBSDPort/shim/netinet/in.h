#ifndef _NETINET_IN_SHIM_H_
#define _NETINET_IN_SHIM_H_

#include <sys/types.h>
#include <sys/socket.h>

typedef __uint32_t in_addr_t;
typedef __uint16_t in_port_t;

#define s6_addr __u6_addr.__u6_addr8

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define IPPROTO_IPV6 41
#define IPPROTO_ICMPV6 58
#define IPPROTO_SCTP 132
#define IPPROTO_RAW 255

#define IP_MAXPACKET 65535

#define ntohl(x) __builtin_bswap32(x)
#define ntohs(x) __builtin_bswap16(x)
#define htonl(x) __builtin_bswap32(x)
#define htons(x) __builtin_bswap16(x)

#define INADDR_ANY 0
#define INADDR_BROADCAST 0xffffffff

#endif
