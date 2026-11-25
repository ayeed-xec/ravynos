#ifndef _NET_IF_MEDIA_SHIM_H_
#define _NET_IF_MEDIA_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>

#define IFM_NMASK 0xe0000000
#define IFM_ETHER 0x00000000
#define IFM_10_T 3
#define IFM_100_TX 6
#define IFM_1000_T 16
#define IFM_10G_T 27
#define IFM_AUTO 0
#define IFM_NONE 0
#define IFM_FDX 0x00100000
#define IFM_HDX 0x00200000

#define IFM_10G_SR 25
#define IFM_10G_LR 26

#define IF_Gbps(x) ((x) * 1000000000ULL)
#define IF_Mbps(x) ((x) * 1000000ULL)
#define IF_Kbps(x) ((x) * 1000ULL)

#define IFM_SUBTYPE(x) ((x) & 0xfff)
#define IFM_TYPE(x) ((x) & IFM_NMASK)
#define IFM_OPTIONS(x) ((x) & 0x001fffff)
#define IFM_INST(x) ((x) >> 28)

#define IFM_AVALID 0x1
#define IFM_ACTIVE 0x2

#define IFM_1000_SX 15

typedef struct ifmedia_entry {
    TAILQ_ENTRY(ifmedia_entry) ifm_list;
    int ifm_media;
    int ifm_status;
} ifmedia_entry;

struct ifmedia {
    TAILQ_HEAD(ifmedia_list, ifmedia_entry) ifm_list;
    struct ifmedia_entry *ifm_cur;
    u_int ifm_mask;
    int ifm_media;
    int ifm_status;
    void *ifm_aux;
};

struct ifmediareq {
    char ifm_name[IFNAMSIZ];
    int ifm_current;
    int ifm_mask;
    int ifm_status;
    int ifm_active;
    int ifm_count;
    int *ifm_ulist;
};

#define IFM_IMASK 0xffffffff

typedef int (*ifm_change_cb_t)(struct ifnet *);
typedef void (*ifm_status_cb_t)(struct ifnet *, struct ifmedia_entry *);

int ifmedia_init(struct ifmedia *ifm, int dontcare_mask, ifm_change_cb_t change,
    ifm_status_cb_t status);
void ifmedia_removeall(struct ifmedia *ifm);
int ifmedia_add(struct ifmedia *ifm, int mword, int data, void *rsp);
void ifmedia_set(struct ifmedia *ifm, int target);
int ifmedia_ioctl(if_t ifp, void *ifr, struct ifmedia *ifm, u_long cmd);

#endif
