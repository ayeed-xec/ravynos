#ifndef _NET_BPF_SHIM_H_
#define _NET_BPF_SHIM_H_

#include <sys/types.h>
#include <net/if.h>

struct mbuf;
struct ifnet;

void bpf_mtap(if_t ifp, struct mbuf *m);
void bpf_mtap2(void *dst, void *src, u_int len, if_t ifp);
void bpfattach(if_t ifp, u_int dlt, u_int hdrlen);

#define DLT_EN10MB 1
#define BPF_TAP(ifp, m) bpf_mtap((ifp), (m))
#define BPF_MTAP(ifp, m) bpf_mtap((ifp), (m))

#endif
