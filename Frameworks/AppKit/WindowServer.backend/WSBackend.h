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

#import <mach/mach.h>
#import <stdint.h>
#import <WindowServer/message.h>
#import <sys/types.h>

kern_return_t WSBackendLookupService(void);
kern_return_t WSBackendRegister(mach_port_t replyPort, const char *bundleID, pid_t pid);
kern_return_t WSBackendSend(unsigned int code, const void *data, unsigned int len);
kern_return_t WSBackendReceive(mach_port_t replyPort, ReceiveMessage *msg, unsigned int timeoutMs);
