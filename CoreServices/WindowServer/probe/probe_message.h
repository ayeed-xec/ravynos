#ifndef WS_PROBE_MESSAGE_H
#define WS_PROBE_MESSAGE_H

#include <mach/mach.h>
#include <stdint.h>

#define WINDOWSERVER_SVC_NAME "com.ravynos.WindowServer"

enum {
    MSG_ID_PORT = 90210,
    MSG_ID_INLINE = 90211,
    MSG_ID_RPC = 90222
};

enum {
    CODE_NULL = 0,
    CODE_PING,
    CODE_PONG
};

typedef struct {
    mach_msg_header_t header;
    unsigned int code;
    unsigned int pid;
    unsigned char data[64];
    unsigned int len;
} ProbeMessage;

extern kern_return_t probe_bootstrap_port(mach_port_t *out);
extern kern_return_t probe_check_in(mach_port_t bootstrap, mach_port_t *out);
extern kern_return_t probe_look_up(mach_port_t bootstrap, mach_port_t *out);

#endif
