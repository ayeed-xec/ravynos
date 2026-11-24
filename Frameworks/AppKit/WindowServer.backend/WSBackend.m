/*
 * Copyright (C) 2026 Shaikh Ayeed Mohd Ali <225547338+ayeed-xec@users.noreply.github.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#import "WSBackend.h"

#import <mach/bootstrap.h>
#import <string.h>
#import <unistd.h>

static mach_port_t wsServicePort = MACH_PORT_NULL;

kern_return_t WSBackendLookupService(void)
{
    if(wsServicePort != MACH_PORT_NULL)
        return KERN_SUCCESS;
    return bootstrap_look_up(bootstrap_port, WINDOWSERVER_SVC_NAME, &wsServicePort);
}

kern_return_t WSBackendRegister(mach_port_t replyPort, const char *bundleID, pid_t pid)
{
    kern_return_t kr = WSBackendLookupService();
    if(kr != KERN_SUCCESS)
        return kr;

    PortMessage msg = {0};
    msg.header.msgh_remote_port = wsServicePort;
    msg.header.msgh_bits = MACH_MSGH_BITS_SET(MACH_MSG_TYPE_COPY_SEND, 0, 0, MACH_MSGH_BITS_COMPLEX);
    msg.header.msgh_id = MSG_ID_PORT;
    msg.header.msgh_size = sizeof(msg);
    msg.msgh_descriptor_count = 1;
    msg.descriptor.type = MACH_MSG_PORT_DESCRIPTOR;
    msg.descriptor.name = replyPort;
    msg.descriptor.disposition = MACH_MSG_TYPE_MAKE_SEND;
    msg.pid = pid;
    strncpy(msg.bundleID, bundleID, sizeof(msg.bundleID) - 1);

    return mach_msg(&msg.header, MACH_SEND_MSG|MACH_SEND_TIMEOUT, sizeof(msg), 0, MACH_PORT_NULL,
        2000 /* ms timeout */, MACH_PORT_NULL);
}

kern_return_t WSBackendSend(unsigned int code, const void *data, unsigned int len)
{
    if(wsServicePort == MACH_PORT_NULL)
        return KERN_FAILURE;

    Message msg = {0};
    msg.header.msgh_remote_port = wsServicePort;
    msg.header.msgh_bits = MACH_MSGH_BITS_SET(MACH_MSG_TYPE_COPY_SEND, 0, 0, 0);
    msg.header.msgh_id = MSG_ID_INLINE;
    msg.header.msgh_size = sizeof(msg);
    msg.code = code;
    msg.pid = getpid();
    if(len > sizeof(msg.data))
        len = sizeof(msg.data);
    memcpy(msg.data, data, len);
    msg.len = len;

    return mach_msg(&msg.header, MACH_SEND_MSG|MACH_SEND_TIMEOUT, sizeof(msg), 0, MACH_PORT_NULL,
        2000 /* ms timeout */, MACH_PORT_NULL);
}

kern_return_t WSBackendReceive(mach_port_t replyPort, ReceiveMessage *msg, unsigned int timeoutMs)
{
    return mach_msg(&msg->header, MACH_RCV_MSG|MACH_RCV_TIMEOUT, 0, sizeof(*msg), replyPort,
        timeoutMs, MACH_PORT_NULL);
}
