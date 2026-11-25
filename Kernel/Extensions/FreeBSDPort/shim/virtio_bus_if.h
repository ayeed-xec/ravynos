#ifndef _VIRTIO_BUS_IF_SHIM_H_
#define _VIRTIO_BUS_IF_SHIM_H_

#include <sys/bus.h>

struct vq_alloc_info;

struct virtio_bus_methods {
    uint64_t (*negotiate_features)(device_t dev, uint64_t child_features);
    int (*with_feature)(device_t dev, uint64_t feature);
    int (*alloc_virtqueues)(device_t dev, int flags, int nvqs,
        struct vq_alloc_info *info);
    int (*setup_intr)(device_t dev, enum intr_type type);
    void (*stop)(device_t dev);
    int (*reinit)(device_t dev, uint64_t features);
    void (*reinit_complete)(device_t dev);
    void (*notify_vq)(device_t dev, uint16_t queue);
    int (*config_generation)(device_t dev);
    void (*read_device_config)(device_t dev, bus_size_t offset, void *dst,
        int len);
    void (*write_device_config)(device_t dev, bus_size_t offset, void *src,
        int len);
    void (*poll)(device_t dev);
};

static inline uint64_t
VIRTIO_BUS_NEGOTIATE_FEATURES(device_t dev, uint64_t child_features)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->negotiate_features(dev, child_features));
}

static inline int
VIRTIO_BUS_WITH_FEATURE(device_t dev, uint64_t feature)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->with_feature(dev, feature));
}

static inline int
VIRTIO_BUS_ALLOC_VIRTQUEUES(device_t dev, int flags, int nvqs,
    struct vq_alloc_info *info)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->alloc_virtqueues(dev, flags, nvqs, info));
}

static inline int
VIRTIO_BUS_SETUP_INTR(device_t dev, enum intr_type type)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->setup_intr(dev, type));
}

static inline void
VIRTIO_BUS_STOP(device_t dev)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->stop(dev);
}

static inline int
VIRTIO_BUS_REINIT(device_t dev, uint64_t features)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->reinit(dev, features));
}

static inline void
VIRTIO_BUS_REINIT_COMPLETE(device_t dev)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->reinit_complete(dev);
}

static inline void
VIRTIO_BUS_NOTIFY_VQ(device_t dev, uint16_t queue)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->notify_vq(dev, queue);
}

static inline int
VIRTIO_BUS_CONFIG_GENERATION(device_t dev)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    return (m->config_generation(dev));
}

static inline void
VIRTIO_BUS_READ_DEVICE_CONFIG(device_t dev, bus_size_t offset, void *dst,
    int len)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->read_device_config(dev, offset, dst, len);
}

static inline void
VIRTIO_BUS_WRITE_DEVICE_CONFIG(device_t dev, bus_size_t offset, void *src,
    int len)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->write_device_config(dev, offset, src, len);
}

static inline void
VIRTIO_BUS_POLL(device_t dev)
{
    struct virtio_bus_methods *m = device_bus_ops(dev, "virtio_bus");

    m->poll(dev);
}

#endif
