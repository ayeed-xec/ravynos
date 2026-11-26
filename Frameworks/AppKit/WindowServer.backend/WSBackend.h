// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors

#import <mach/mach.h>
#import <stdint.h>
#import <WindowServer/message.h>
#import <sys/types.h>

kern_return_t WSBackendLookupService(void);
kern_return_t WSBackendRegister(mach_port_t replyPort, const char *bundleID, pid_t pid);
kern_return_t WSBackendSend(unsigned int code, const void *data, unsigned int len);
kern_return_t WSBackendReceive(mach_port_t replyPort, ReceiveMessage *msg, unsigned int timeoutMs);
