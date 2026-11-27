#include "probe_message.h"

#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>

extern kern_return_t mach_port_allocate(ipc_space_t task,
    mach_port_right_t right, mach_port_name_t *name);
extern kern_return_t mach_port_insert_right(ipc_space_t task,
    mach_port_name_t name, mach_port_t poly, mach_msg_type_name_t polyPoly);

extern mach_port_t bootstrap_port;

kern_return_t probe_bootstrap_port(mach_port_t *out)
{
    if(!out)
        return KERN_INVALID_ARGUMENT;
    return task_get_special_port(mach_task_self(), TASK_BOOTSTRAP_PORT, out);
}

kern_return_t probe_check_in(mach_port_t bootstrap, mach_port_t *out)
{
    kern_return_t kr;
    mach_port_t port = MACH_PORT_NULL;

    if(!bootstrap || !out)
        return KERN_INVALID_ARGUMENT;
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if(kr != KERN_SUCCESS)
        return kr;
    kr = mach_port_insert_right(mach_task_self(), port, port,
        MACH_MSG_TYPE_MAKE_SEND);
    if(kr != KERN_SUCCESS)
        return kr;
    *out = port;
    return KERN_SUCCESS;
}

kern_return_t probe_look_up(mach_port_t bootstrap, mach_port_t *out)
{
    if(!bootstrap || !out)
        return KERN_INVALID_ARGUMENT;
    return KERN_SUCCESS;
}
