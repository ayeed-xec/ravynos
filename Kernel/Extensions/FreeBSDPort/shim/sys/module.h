#ifndef _SYS_MODULE_SHIM_H_
#define _SYS_MODULE_SHIM_H_

#include <sys/kernel.h>

typedef struct module *module_t;
typedef int (*modeventhandl_t)(module_t mod, int what, void *arg);

#define MOD_LOAD 1
#define MOD_UNLOAD 2
#define MOD_SHUTDOWN 3
#define MOD_QUIESCE 4

struct moduledata {
    const char *name;
    modeventhandl_t evh;
    void *arg;
};

typedef struct moduledata moduledata_t;

typedef struct device_method {
    const char *name;
    void *func;
} device_method_t;

struct driver {
    const char *name;
    device_method_t *methods;
    size_t size;
};

typedef struct driver driver_t;

#define MODULE_VERSION(mod, ver)
#define MODULE_DEPEND(mod, depend, min, pref, max)

#define DECLARE_MODULE(name, data, sub, order) \
    module_t name##_shim_module = (module_t)&data

#define DRIVER_MODULE(name, bus, driver, devclass, evh, arg) \
    struct moduledata name##_##bus##_shim_moddata = { \
        #name "_" #bus, (modeventhandl_t)(uintptr_t)(evh), \
        (void *)(uintptr_t)(arg) \
    }; \
    module_t name##_##bus##_shim_module = &name##_##bus##_shim_moddata

#endif
