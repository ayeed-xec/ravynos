#ifndef _NET_IF_VAR_SHIM_H_
#define _NET_IF_VAR_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <net/if.h>

struct ifaddr;
struct ifmultiaddr;
struct mbuf;
struct ifmedia;

typedef TAILQ_HEAD(ifaddrhead, ifaddr) ifaddrhead_t;
typedef TAILQ_HEAD(ifmultihead, ifmultiaddr) ifmultihead_t;

struct ifaltq;

typedef void (*if_start_fn_t)(if_t);
typedef int (*if_ioctl_fn_t)(if_t, u_long, caddr_t);
typedef void (*if_init_fn_t)(void *);
typedef int (*if_transmit_fn_t)(if_t, struct mbuf *);
typedef void (*if_qflush_fn_t)(if_t);
typedef void (*if_input_fn_t)(if_t, struct mbuf *);
typedef void (*if_dma_flush_fn_t)(if_t);

struct ifaltq {
    struct mbuf *ifq_head;
    struct mbuf *ifq_tail;
    int ifq_len;
    int ifq_maxlen;
    int ifq_drv_len;
    int ifq_drv_maxlen;
};

struct ifnet {
    void *if_softc;
    void *if_llsoftc;
    void *if_l2com;
    TAILQ_ENTRY(ifnet) if_link;
    char if_xname[IFNAMSIZ];
    const char *if_dname;
    int if_dunit;
    u_int if_flags;
    u_int if_drv_flags;
    u_int if_capabilities;
    u_int if_capenable;
    u_long if_hwassist;
    int if_hdrlen;
    int if_mtu;
    u_long if_baudrate;
    struct ifaddrhead if_addrhead;
    struct ifmultihead if_multiaddrs;
    struct ifaltq if_snd;
    if_link_state if_link_state;
    if_counter_fn if_get_counter;
    if_init_fn_t if_init;
    if_ioctl_fn_t if_ioctl;
    if_start_fn_t if_start;
    if_transmit_fn_t if_transmit;
    if_qflush_fn_t if_qflush;
    if_input_fn_t if_input;
    void *if_vlantrunk;
    struct ifmedia *if_media_ptr;
    u_int if_hw_tsomax;
    u_int if_hw_tsomaxsegcount;
    u_int if_hw_tsomaxsegsize;
    struct sockaddr_dl if_lladdr;
};

#define IF_LLADDR(ifp) LLADDR(&(ifp)->if_lladdr)

#define IFQ_IS_EMPTY(ifq) ((ifq)->ifq_len == 0)
#define IFQ_DRV_IS_EMPTY(ifq) ((ifq)->ifq_len == 0)
#define IFQ_SET_MAXLEN(ifq, len) do { \
    (ifq)->ifq_maxlen = (len); \
    (ifq)->ifq_drv_maxlen = 0; \
} while (0)
#define IFQ_SET_READY(ifq) do { \
    (ifq)->ifq_drv_maxlen = 0; \
} while (0)

#define IF_ADDR_LOCK(ifp) if_addr_lock((ifp))
#define IF_ADDR_UNLOCK(ifp) if_addr_unlock((ifp))
#define IF_ADDR_RLOCK(ifp) if_addr_lock((ifp))
#define IF_ADDR_RUNLOCK(ifp) if_addr_unlock((ifp))
#define IF_ADDR_WLOCK(ifp) if_addr_lock((ifp))
#define IF_ADDR_WUNLOCK(ifp) if_addr_unlock((ifp))
#define IF_MADDR_RLOCK(ifp) if_addr_lock((ifp))
#define IF_MADDR_RUNLOCK(ifp) if_addr_unlock((ifp))
#define IF_MADDR_WLOCK(ifp) if_addr_lock((ifp))
#define IF_MADDR_WUNLOCK(ifp) if_addr_unlock((ifp))

typedef struct device *device_t;
struct sysctl_ctx_list *device_get_sysctl_ctx(device_t dev);
struct sysctl_oid *device_get_sysctl_tree(device_t dev);
void if_addr_rlock(if_t ifp);
void if_addr_runlock(if_t ifp);
void if_maddr_rlock(if_t ifp);
void if_maddr_runlock(if_t ifp);
#define IF_LOCK(ifp) if_addr_lock((ifp))
#define IF_UNLOCK(ifp) if_addr_unlock((ifp))

#define IFT_OTHER 0x1

void if_addr_lock(if_t ifp);
void if_addr_unlock(if_t ifp);

if_t if_alloc(u_char type);
void if_free(if_t ifp);
void if_initname(if_t ifp, const char *name, int unit);
void if_attach(if_t ifp);
void if_detach(if_t ifp);
void if_setsoftc(if_t ifp, void *softc);
void *if_getsoftc(if_t ifp);
void if_printf(if_t ifp, const char *fmt, ...) __printflike(2, 3);
void if_setmtu(if_t ifp, int mtu);
void if_sethwassist(if_t ifp, u_long hwassist);
void if_setcapabilities(if_t ifp, int caps);
void if_setcapenable(if_t ifp, int caps);
void if_sethdrlen(if_t ifp, int hdrlen);
void if_setinitfn(if_t ifp, if_init_fn_t fn);
void if_setioctlfn(if_t ifp, if_ioctl_fn_t fn);
void if_setstartfn(if_t ifp, if_start_fn_t fn);
void if_settransmitfn(if_t ifp, if_transmit_fn_t fn);
void if_setqflushfn(if_t ifp, if_qflush_fn_t fn);
void if_setinputfn(if_t ifp, if_input_fn_t fn);
void if_setifheaderlen(if_t ifp, int len);
void if_setbaudrate(if_t ifp, u_long baudrate);
void if_setdev(if_t ifp, void *dev);
void if_qflush(if_t ifp);
void if_input(if_t ifp, struct mbuf *m);
void if_start(if_t ifp);
void if_link_state_change(if_t ifp, int link_state);
uint64_t if_get_counter_default(if_t ifp, ift_counter cnt);
int if_ioctl(if_t ifp, u_long cmd, caddr_t data);
int if_output(if_t ifp, struct mbuf *m);
int if_getmtu(if_t ifp);
int if_getmtu_family(if_t ifp, int family);
u_long if_gethwassist(if_t ifp);
int if_getcapabilities(if_t ifp);
int if_getcapenable(if_t ifp);
int if_getdunit(if_t ifp);
const char *if_getdname(if_t ifp);
const char *if_getname(if_t ifp);
int if_getindex(if_t ifp);
struct ifaddr *ifaddr_byindex(int idx);
struct ifnet *ifnet_byindex(int idx);
int if_addmulti(if_t ifp, struct sockaddr *sa, struct ifmultiaddr **ifmap);
int if_delmulti(if_t ifp, struct sockaddr *sa);
void if_vmove(if_t ifp);

struct buf_ring;

int drbr_enqueue(if_t ifp, struct buf_ring *br, struct mbuf *m);
struct mbuf *drbr_peek(if_t ifp, struct buf_ring *br);
void drbr_putback(if_t ifp, struct buf_ring *br, struct mbuf *m);
void drbr_advance(if_t ifp, struct buf_ring *br);
struct mbuf *drbr_dequeue(if_t ifp, struct buf_ring *br);
int drbr_empty(if_t ifp, struct buf_ring *br);
void drbr_flush(if_t ifp, struct buf_ring *br);

#include <net/if_addr.h>

#endif
