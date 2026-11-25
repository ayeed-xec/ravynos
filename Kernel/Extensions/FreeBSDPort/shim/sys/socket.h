#ifndef _SYS_SOCKET_SHIM_H_
#define _SYS_SOCKET_SHIM_H_

#include <sys/types.h>

typedef __uint8_t sa_family_t;
typedef __uint16_t in_port_t;
typedef __uint32_t in_addr_t;

#define AF_UNSPEC 0
#define AF_LOCAL 1
#define AF_UNIX AF_LOCAL
#define AF_INET 2
#define AF_INET6 28
#define AF_LINK 18

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define PF_UNSPEC AF_UNSPEC
#define PF_LOCAL AF_LOCAL
#define PF_INET AF_INET
#define PF_INET6 AF_INET6
#define PF_LINK AF_LINK

#define SOL_SOCKET 0xffff
#define SO_RCVBUF 0x1002
#define SO_SNDBUF 0x1001

struct in_addr {
    in_addr_t s_addr;
};

struct in6_addr {
    union {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __u6_addr;
};

struct sockaddr {
    unsigned char sa_len;
    sa_family_t sa_family;
    char sa_data[14];
};

struct sockaddr_dl {
    unsigned char sdl_len;
    sa_family_t sdl_family;
    unsigned short sdl_index;
    unsigned char sdl_type;
    unsigned char sdl_nlen;
    unsigned char sdl_alen;
    unsigned char sdl_slen;
    char sdl_data[46];
};

#define LLADDR(s) ((s)->sdl_data + (s)->sdl_nlen)

struct sockaddr_in {
    unsigned char sin_len;
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

struct sockaddr_in6 {
    unsigned char sin6_len;
    sa_family_t sin6_family;
    in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t sin6_scope_id;
};

#endif
