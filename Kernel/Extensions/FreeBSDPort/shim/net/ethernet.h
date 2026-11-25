#ifndef _NET_ETHERNET_SHIM_H_
#define _NET_ETHERNET_SHIM_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_var.h>

#define ETHER_ADDR_LEN 6
#define ETHER_TYPE_LEN 2
#define ETHER_CRC_LEN 4
#define ETHER_HDR_LEN (ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN)
#define ETHER_MIN_LEN 64
#define ETHER_MAX_LEN 1518
#define ETHER_MAX_LEN_JUMBO 9018
#define ETHER_VLAN_ENCAP_LEN 4
#define ETHER_MTU (ETHER_MAX_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)
#define ETHERMIN (ETHER_MIN_LEN - ETHER_CRC_LEN)
#define ETHER_MAX_DIX_LEN 65535
#define ETHER_MAX_JUMBO_FRAME_SIZE (ETHER_MAX_LEN_JUMBO - ETHER_CRC_LEN)

struct ether_addr {
    unsigned char octet[ETHER_ADDR_LEN];
};

struct ether_header {
    struct ether_addr ether_dhost;
    struct ether_addr ether_shost;
    uint16_t ether_type;
};

#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_IPV6 0x86dd
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_VLAN 0x8100
#define ETHERTYPE_QINQ 0x88a8
#define ETHERTYPE_FLOWCONTROL 0x8808
#define ETHERTYPE_PAUSE 0x8808

struct ether_vlan_header {
    struct ether_addr evl_dhost;
    struct ether_addr evl_shost;
    uint16_t evl_encap_proto;
    uint16_t evl_tag;
    uint16_t evl_proto;
};

#define ETHER_BPF_MTAP(ifp, m) bpf_mtap((ifp), (m))

struct mbuf;
struct ifnet;
void bpf_mtap(if_t ifp, struct mbuf *m);
void ether_ifattach(if_t ifp, const uint8_t *lladdr);
void ether_ifdetach(if_t ifp);
int ether_ioctl(if_t ifp, u_long cmd, caddr_t data);
struct mbuf *ether_vlanencap(struct mbuf *m, uint16_t vid);

#endif
