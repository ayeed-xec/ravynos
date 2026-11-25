#ifndef _SYS_MBUF_SHIM_H_
#define _SYS_MBUF_SHIM_H_

#include <sys/types.h>
#include <sys/param.h>
#include <sys/queue.h>

struct ifnet;

#define MT_DATA 1
#define MT_HEADER 2
#define MT_NOINIT 7

typedef void *m_mclasstype_t;

struct pkthdr {
    struct ifnet *rcvif;
    int len;
    int csum_flags;
    uint32_t csum_data;
    uint32_t flowid;
    uint8_t flowid_type;
    uint16_t tso_segsz;
    uint16_t ether_vtag;
};

struct mbuf_ext {
    void *ext_buf;
    size_t ext_size;
};

struct mbuf {
    struct mbuf *m_next;
    struct mbuf *m_nextpkt;
    caddr_t m_data;
    int m_len;
    uint32_t m_flags;
    short m_type;
    struct pkthdr m_pkthdr;
    struct mbuf_ext *m_ext;
    char m_ibuf[MSIZE];
};

#define mtod(m, t) ((t)((m)->m_data))

#define M_EXT 0x00000001
#define M_PKTHDR 0x00000002
#define M_EOR 0x00000004
#define M_RDONLY 0x00000010
#define M_PROTO1 0x00000020
#define M_PROTO2 0x00000040
#define M_PROTO3 0x00000080
#define M_PROTO4 0x00000100
#define M_PROTO5 0x00000200
#define M_BCAST 0x00000400
#define M_MCAST 0x00000800
#define M_FRAG 0x00001000
#define M_FIRSTFRAG 0x00002000
#define M_LASTFRAG 0x00004000
#define M_NOFREE 0x00008000
#define M_VLANTAG 0x00010000
#define M_PROMISC 0x00020000
#define M_SKIP_FIREWALL 0x00040000
#define M_HASHTYPE_NONE 0
#define M_HASHTYPE_OPAQUE 1
#define M_HASHTYPE_OPAQUE_HASH M_HASHTYPE_OPAQUE

#define M_HASHTYPE_GET(m) ((m)->m_pkthdr.flowid_type)
#define M_HASHTYPE_SET(m, v) do { (m)->m_pkthdr.flowid_type = (v); } while (0)

#define M_ASSERTPKTHDR(m) KASSERT((m) != NULL && \
    ((m)->m_flags & M_PKTHDR), ("mbuf lacks packet header"))

#define MPASS(cond) KASSERT(cond, ("MPASS failed: " #cond))

#define __predict_true(exp) __builtin_expect((exp) != 0, 1)
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)

#define M_TRAILINGSPACE(m) \
    ((m)->m_ext != NULL ? \
    (int)((m)->m_ext->ext_buf + (m)->m_ext->ext_size - \
    ((m)->m_data + (m)->m_len)) : 0)

#define M_LEADINGSPACE(m) ((int)((m)->m_data - (caddr_t)(m)->m_ibuf))

#define M_WAIT M_WAITOK
#define M_DONTWAIT M_NOWAIT
#define M_TRYWAIT M_WAITOK

#define CSUM_IP 0x00000001
#define CSUM_TCP 0x00000002
#define CSUM_UDP 0x00000004
#define CSUM_TCP_IPV6 0x00000008
#define CSUM_UDP_IPV6 0x00000010
#define CSUM_FRAG_NOT_CHECKED 0x00000020
#define CSUM_IP_FRAGS 0x00000040
#define CSUM_IPV6 0x00000080
#define CSUM_IP_CHECKED 0x00000100
#define CSUM_IP_VALID 0x00000200
#define CSUM_DATA_VALID 0x00000400
#define CSUM_PSEUDO_HDR 0x00000800
#define CSUM_SCTP 0x00001000
#define CSUM_SCTP_IPV6 0x00002000
#define CSUM_TSO 0x00004000
#define CSUM_IP_TSO 0x00008000
#define CSUM_IP6_TSO 0x00010000

struct mbuf *m_get(int how, short type);
struct mbuf *m_gethdr(int how, short type);
struct mbuf *m_getcl(int how, short type, int flags);
struct mbuf *m_getjcl(int how, short type, int flags, int size);
struct mbuf *m_free(struct mbuf *m);
void m_freem(struct mbuf *m);
void m_adj(struct mbuf *m, int len);
void m_cat(struct mbuf *m, struct mbuf *n);
struct mbuf *m_pullup(struct mbuf *m, int len);
int m_length(struct mbuf *m, struct mbuf **last);
u_int m_fixhdr(struct mbuf *m);
void m_copydata(struct mbuf *m, int offset, int len, caddr_t dst);
struct mbuf *m_copym(struct mbuf *m, int offset, int len, int how);

struct mbuf *m_defrag(struct mbuf *m, int how);
struct mbuf *m_collapse(struct mbuf *m, int how, int maxfrags);

#endif
