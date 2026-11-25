#ifndef _VIRTIO_IF_SHIM_H_
#define _VIRTIO_IF_SHIM_H_

#include <sys/bus.h>

struct virtio_methods {
    int (*attach_completed)(device_t dev);
    int (*config_change)(device_t dev);
};

static inline int
VIRTIO_ATTACH_COMPLETED(device_t dev)
{
    struct virtio_methods *m = device_bus_ops(dev, "virtio");

    return (m->attach_completed(dev));
}

static inline int
VIRTIO_CONFIG_CHANGE(device_t dev)
{
    struct virtio_methods *m = device_bus_ops(dev, "virtio");

    return (m->config_change(dev));
}

#endif
