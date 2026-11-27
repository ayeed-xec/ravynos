#include "probe_message.h"

#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>

extern mach_port_t bootstrap_port;

int probe_client_main(void)
{
    kern_return_t kr;
    mach_port_t bootstrap = MACH_PORT_NULL;
    mach_port_t send = MACH_PORT_NULL;
    mach_port_t recv = MACH_PORT_NULL;
    ProbeMessage msg;
    unsigned int round = 0;
    int rtt = 0;

    kr = probe_bootstrap_port(&bootstrap);
    if(kr != KERN_SUCCESS)
        return 1;
    kr = probe_check_in(bootstrap, &recv);
    if(kr != KERN_SUCCESS)
        return 2;

    send = recv;
    for(round = 0; round < 4; round++) {
        msg.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
        msg.header.msgh_size = sizeof(msg);
        msg.header.msgh_remote_port = send;
        msg.header.msgh_id = MSG_ID_INLINE;
        msg.code = CODE_PING;
        msg.pid = round;
        msg.len = 4;
        kr = mach_msg(&msg.header, MACH_SEND_MSG, sizeof(msg), 0,
            MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        if(kr != KERN_SUCCESS)
            return 3;
        kr = mach_msg(&msg.header, MACH_RCV_MSG, 0, sizeof(msg), recv,
            MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        if(kr != KERN_SUCCESS)
            return 4;
        if(msg.code == CODE_PONG)
            rtt++;
    }
    return rtt == 4 ? 0 : 5;
}
