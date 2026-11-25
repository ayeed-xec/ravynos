#ifndef _SYS_BUS_SHIM_H_
#define _SYS_BUS_SHIM_H_

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/sysctl.h>
#include <machine/bus.h>
#include <machine/resource.h>

typedef struct device *device_t;
typedef struct devclass *devclass_t;

#define DEVMETHOD(method, func) { #method, (void *)(func) }
#define DEVMETHOD_END { NULL, NULL }

#define DEFINE_CLASS_0(name, driver, methods, size) \
    static driver_t driver = { #name, methods, size }

struct device {
    device_t dev_parent;
    driver_t *dev_driver;
    void *dev_softc;
    size_t dev_softcsize;
    void *dev_ivars;
    void *dev_bus_ops;
    void *dev_child_ops;
    int dev_unit;
    int dev_flags;
    char dev_nameunit[32];
    char dev_desc[64];
    sysctl_ctx_list_t dev_sysctl_ctx;
    struct sysctl_oid *dev_sysctl_tree;
};

#define DS_NOTPRESENT 10
#define DS_ALIVE 20
#define DS_ATTACHED 30
#define DS_BUSY 40

typedef int device_state_t;

device_t device_add_child(device_t dev, const char *name, int unit);
device_state_t device_get_state(device_t dev);
int device_is_attached(device_t dev);
int device_probe(device_t dev);
int device_attach(device_t dev);
device_t device_get_parent(device_t dev);
void *device_get_softc(device_t dev);
void device_set_softc(device_t dev, void *softc, size_t size);
const char *device_get_desc(device_t dev);
void device_set_desc(device_t dev, const char *desc);
void device_set_desc_copy(device_t dev, const char *desc);
int device_get_unit(device_t dev);
const char *device_get_name(device_t dev);
const char *device_get_nameunit(device_t dev);
void *device_get_ivars(device_t dev);
void device_set_ivars(device_t dev, void *ivars);
int device_printf(device_t dev, const char *fmt, ...)
    __printflike(2, 3);
int device_is_alive(device_t dev);
void device_quiesce(device_t dev);
void device_busy(device_t dev);
void device_unbusy(device_t dev);
int device_delete_child(device_t dev, device_t child);
int device_detach(device_t dev);
void *device_bus_ops(device_t dev, const char *ifname);

int bus_generic_attach(device_t dev);
int bus_generic_detach(device_t dev);
int bus_generic_suspend(device_t dev);
int bus_generic_resume(device_t dev);
int bus_generic_probe(device_t dev);
void bus_generic_shutdown(device_t dev);
void bus_topo_lock(void);
void bus_topo_unlock(void);

struct resource *bus_alloc_resource_any(device_t dev, int type, int *rid,
    u_int flags);
int bus_release_resource(device_t dev, int type, int rid,
    struct resource *r);
int bus_setup_intr(device_t dev, struct resource *r, int flags,
    void (*filter)(void *), void (*handler)(void *), void *arg,
    void **cookiep);
int bus_teardown_intr(device_t dev, struct resource *r, void *cookie);
bus_dma_tag_t bus_get_dma_tag(device_t dev);
int bus_child_present(device_t dev);
int BUS_READ_IVAR(device_t dev, device_t child, int which, uintptr_t *result);
int BUS_WRITE_IVAR(device_t dev, device_t child, int which, uintptr_t value);

typedef enum intr_type {
    INTR_MPSAFE = 1,
    INTR_TYPE_TTY = 2,
    INTR_TYPE_BIO = 4,
    INTR_TYPE_NET = 8,
    INTR_TYPE_MISC = 16,
    INTR_TYPE_CLK = 32
} intr_type;

#endif
