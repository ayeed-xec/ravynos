/* Minimal Mach header shim for building Apple's migcom on Linux.
 *
 * Mirrors the subset of xnu's osfmk/mach/message.h (SPDX:APSL-2.0)
 * that migcom uses. The sizes and values below are baked into the
 * stubs migcom generates, so they must stay byte-for-byte identical
 * to the kernel headers (xnu asserts: mach_msg_header_t = 24,
 * mach_msg_body_t = 4, NDR_record_t = 8). */
#ifndef _MACH_MESSAGE_H_SHIM_
#define _MACH_MESSAGE_H_SHIM_

#include <mach/kern_return.h>

typedef unsigned int    natural_t;
typedef int             integer_t;
typedef unsigned int    mach_msg_bits_t;
typedef integer_t       mach_msg_id_t;
typedef natural_t       mach_msg_size_t;
typedef natural_t       mach_msg_type_number_t;
typedef natural_t       mach_msg_type_size_t;
typedef natural_t       mach_msg_type_name_t;
typedef natural_t       mach_msg_copy_options_t;
typedef natural_t       mach_msg_guard_flags_t;
typedef natural_t       mach_port_t;
typedef natural_t       mach_port_name_t;
typedef int             ipc_space_t;

typedef enum {
        MACH_MSG_PORT_DESCRIPTOR                = 0,
        MACH_MSG_OOL_DESCRIPTOR                 = 1,
        MACH_MSG_OOL_PORTS_DESCRIPTOR           = 2,
        MACH_MSG_GUARDED_PORT_DESCRIPTOR        = 3,
        MACH_MSG_MSG_DESCRIPTOR                 = 4
} mach_msg_descriptor_type_t;

/* MACH_MSG_TYPE_* name constants, mirrored from xnu's
 * osfmk/mach/message.h. */
#define MACH_MSG_TYPE_MOVE_RECEIVE      16      /* Must hold receive right */
#define MACH_MSG_TYPE_MOVE_SEND         17      /* Must hold send right */
#define MACH_MSG_TYPE_MOVE_SEND_ONCE    18      /* Must hold sendonce right */
#define MACH_MSG_TYPE_COPY_SEND         19      /* Must hold send right(s) */
#define MACH_MSG_TYPE_PORT_NAME         15      /* Must hold port right name */
#define MACH_MSG_TYPE_MAKE_SEND         20      /* Must hold receive right */
#define MACH_MSG_TYPE_MAKE_SEND_ONCE    21      /* Must hold receive right */

#define MACH_MSG_TYPE_PORT_RECEIVE      MACH_MSG_TYPE_MOVE_RECEIVE
#define MACH_MSG_TYPE_PORT_SEND         MACH_MSG_TYPE_MOVE_SEND
#define MACH_MSG_TYPE_PORT_SEND_ONCE    MACH_MSG_TYPE_MOVE_SEND_ONCE
#define MACH_MSG_TYPE_POLYMORPHIC       ((mach_msg_type_name_t) -1)

/* Is a given item a port type? (xnu osfmk/mach/message.h) */
#define MACH_MSG_TYPE_PORT_ANY(x)                       \
        (((x) >= MACH_MSG_TYPE_MOVE_RECEIVE) &&         \
         ((x) <= MACH_MSG_TYPE_MAKE_SEND_ONCE))
#define MACH_MSG_TYPE_PORT_ANY_SEND(x)                  \
        (((x) >= MACH_MSG_TYPE_MOVE_SEND) &&            \
         ((x) <= MACH_MSG_TYPE_MAKE_SEND_ONCE))
#define MACH_MSG_TYPE_PORT_ANY_SEND_ONCE(x)             \
        (((x) == MACH_MSG_TYPE_MOVE_SEND_ONCE) ||       \
         ((x) == MACH_MSG_TYPE_MAKE_SEND_ONCE))

typedef struct {
        mach_msg_bits_t         msgh_bits;
        mach_msg_size_t         msgh_size;
        mach_port_t             msgh_remote_port;
        mach_port_t             msgh_local_port;
        mach_port_t             msgh_voucher_port;
        mach_msg_id_t           msgh_id;
} mach_msg_header_t;

typedef struct {
        mach_msg_size_t         msgh_descriptor_count;
} mach_msg_body_t;

#endif /* _MACH_MESSAGE_H_SHIM_ */
