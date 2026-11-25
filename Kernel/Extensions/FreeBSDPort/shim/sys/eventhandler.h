#ifndef _SYS_EVENTHANDLER_SHIM_H_
#define _SYS_EVENTHANDLER_SHIM_H_

#include <sys/types.h>

typedef void *eventhandler_tag;

struct eventhandler_entry;
typedef struct eventhandler_entry *eventhandler_tag_t;

#define EVENTHANDLER_REGISTER(name, func, arg, priority) \
    shim_eventhandler_register(#name, (void *)(func), (arg))

#define EVENTHANDLER_DEREGISTER(name, tag) \
    shim_eventhandler_deregister(#name, (tag))

#define EVENTHANDLER_INVOKE(name, ...) \
    shim_eventhandler_invoke(#name)

eventhandler_tag shim_eventhandler_register(const char *name, void *func,
    void *arg);
void shim_eventhandler_deregister(const char *name, eventhandler_tag tag);
void shim_eventhandler_invoke(const char *name);

#endif
