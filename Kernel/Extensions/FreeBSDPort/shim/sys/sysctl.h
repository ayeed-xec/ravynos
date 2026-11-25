#ifndef _SYS_SYSCTL_SHIM_H_
#define _SYS_SYSCTL_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>

struct sysctl_ctx_list;

struct sysctl_oid;
typedef struct sysctl_oid *sysctl_oid_t;

struct sysctl_req;
typedef struct sysctl_req *sysctl_req_t;

typedef int sysctl_oid_list_dummy;

struct sysctl_oid_list {
    struct sysctl_oid *slh_first;
};

struct sysctl_ctx_entry {
    TAILQ_ENTRY(sysctl_ctx_entry) x_link;
    struct sysctl_oid *x_oid;
    const char *x_name;
};

typedef TAILQ_HEAD(sysctl_ctx_list_head, sysctl_ctx_entry) sysctl_ctx_list_t;

struct sysctl_oid {
    const char *oid_name;
    int oid_number;
    int oid_kind;
    void *oid_arg1;
    int (*oid_arg2)(sysctl_req_t);
    const char *oid_descr;
    struct sysctl_oid_list oid_children;
};

#define CTLTYPE_NODE 1
#define CTLTYPE_INT 2
#define CTLTYPE_STRING 3
#define CTLTYPE_QUAD 4
#define CTLTYPE_OPAQUE 5
#define CTLTYPE_UINT 6
#define CTLTYPE_UQUAD 7

#define CTLFLAG_RD 0x1
#define CTLFLAG_WR 0x2
#define CTLFLAG_RW (CTLFLAG_RD | CTLFLAG_WR)
#define CTLFLAG_DYN 0x10
#define CTLFLAG_ANYBODY 0x100
#define CTLFLAG_SECURE 0x200
#define CTLFLAG_NEEDGIANT 0

#define CTL_SYSCTL 0
#define CTL_KERN 1
#define CTL_VM 2
#define CTL_VFS 3
#define CTL_NET 4
#define CTL_DEV 5
#define CTL_HW 6
#define CTL_MACHDEP 7
#define CTL_USER 8
#define CTL_DEBUG 9

#define CTLFLAG_NOLOCK 0

struct sysctl_ctx_list;
struct sysctl_ctx_entry *sysctl_ctx_entry_add(struct sysctl_ctx_list *clist,
    struct sysctl_oid *oidp);
struct sysctl_oid *sysctl_add_oid(struct sysctl_ctx_list *clist,
    struct sysctl_oid *parent, int number, const char *name, int kind,
    void *arg1, intptr_t arg2, int (*handler)(sysctl_req_t),
    const char *fmt, const char *descr);

int sysctl_ctx_init(sysctl_ctx_list_t *clist);
int sysctl_ctx_free(sysctl_ctx_list_t *clist);

int sysctl_handle_int(sysctl_req_t req);
int sysctl_handle_string(sysctl_req_t req);
int sysctl_handle_quad(sysctl_req_t req);
int sysctl_handle_opaque(sysctl_req_t req);

#define SYSCTL_HANDLER req

#define SYSCTL_CHILDREN(oid) (&(oid)->oid_children)

#define SYSCTL_ADD_NODE(ctx, parent, number, name, access, handler, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_NODE | (access), NULL, 0, (handler), "N", (descr))

#define SYSCTL_ADD_INT(ctx, parent, number, name, access, ptr, val, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_INT | (access), (void *)(uintptr_t)(ptr), (val), \
    sysctl_handle_int, "I", (descr))

#define SYSCTL_ADD_UINT(ctx, parent, number, name, access, ptr, val, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_UINT | (access), (void *)(uintptr_t)(ptr), (val), \
    sysctl_handle_int, "IU", (descr))

#define SYSCTL_ADD_UQUAD(ctx, parent, number, name, access, ptr, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_UQUAD | (access), (void *)(uintptr_t)(ptr), 0, \
    sysctl_handle_quad, "QU", (descr))

#define SYSCTL_ADD_QUAD(ctx, parent, number, name, access, ptr, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_UQUAD | (access), (void *)(uintptr_t)(ptr), 0, \
    sysctl_handle_quad, "Q", (descr))

#define SYSCTL_ADD_STRING(ctx, parent, number, name, access, ptr, len, \
    descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_STRING | (access), (ptr), (len), sysctl_handle_string, "A", \
    (descr))

#define SYSCTL_ADD_OPAQUE(ctx, parent, number, name, access, ptr, len, fmt, \
    descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), \
    CTLTYPE_OPAQUE | (access), (ptr), (len), sysctl_handle_opaque, (fmt), \
    (descr))

#define SYSCTL_ADD_PROC(ctx, parent, number, name, access, arg1, arg2, \
    handler, fmt, descr) \
    sysctl_add_oid((ctx), (parent), (number), (name), (access), (arg1), \
    (arg2), (handler), (fmt), (descr))

#define SYSCTL_ROOT_NODE(number, name, access, handler, descr) \
    sysctl_add_oid(NULL, NULL, (number), (name), CTLTYPE_NODE | (access), \
    NULL, 0, (handler), "N", (descr))

#define __SYSCTL_UNIQ2(x, y) x ## y
#define __SYSCTL_UNIQ(x, y) __SYSCTL_UNIQ2(x, y)
#define __SYSCTL_NAME(line) __SYSCTL_UNIQ(sysctl_shim_oid_, line)

#define SYSCTL_NODE(parent, number, name, access, handler, descr) \
    static struct sysctl_oid_list sysctl__##parent##_##name##_children = { \
        NULL, NULL \
    }; \
    static struct sysctl_oid sysctl__##parent##_##name = { \
        #name, (number), CTLTYPE_NODE | (access), NULL, (handler), \
        (descr), { NULL, NULL } \
    }

#define SYSCTL_INT(parent, number, name, access, ptr, val, descr) \
    static struct sysctl_oid sysctl__##parent##_##name = { \
        #name, (number), CTLTYPE_INT | (access), (void *)(uintptr_t)(ptr), \
        sysctl_handle_int, (descr), { NULL, NULL } \
    }

#define SYSCTL_UQUAD(parent, number, name, access, ptr, descr) \
    static struct sysctl_oid sysctl__##parent##_##name = { \
        #name, (number), CTLTYPE_UQUAD | (access), (void *)(uintptr_t)(ptr), \
        sysctl_handle_quad, (descr), { NULL, NULL } \
    }

#define CTLFLAG_RDTUN (CTLFLAG_RD | 0x80000)
#define OID_AUTO (-1)

#define SYSCTL_DECL(label) extern struct sysctl_oid *sysctl_shim_##label

#endif
