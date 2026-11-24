#include "probe_message.h"

#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>

extern mach_port_t bootstrap_port;

int probe_server_main(void)
{
    kern_return_t kr;
    mach_port_t bootstrap = MACH_PORT_NULL;
    mach_port_t recv = MACH_PORT_NULL;
    ProbeMessage msg;
    unsigned int pings = 0;

    kr = probe_bootstrap_port(&bootstrap);
    if(kr != KERN_SUCCESS)
        return 1;
    kr = probe_check_in(bootstrap, &recv);
    if(kr != KERN_SUCCESS)
        return 2;

    for(;;) {
        msg.header.msgh_size = sizeof(msg);
        kr = mach_msg(&msg.header, MACH_RCV_MSG, 0, sizeof(msg), recv,
            MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        if(kr != KERN_SUCCESS)
            return 3;
        if(msg.header.msgh_id != MSG_ID_INLINE)
            continue;
        if(msg.code == CODE_PING) {
            msg.code = CODE_PONG;
            msg.pid = pings++;
            msg.header.msgh_bits =
                MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
            mach_msg(&msg.header, MACH_SEND_MSG, sizeof(msg), 0,
                MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        }
    }
}
