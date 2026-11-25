#ifndef _SHIM_KERNEL_BASE_TYPES_H
#define _SHIM_KERNEL_BASE_TYPES_H

/*
 * Stand-in for the BSD sys/types.h base types that xnu kernel headers
 * expect from the SDK's clang modules (USE_CLANG_TYPES) in the real
 * build. Clean-room; matches the bsd/sys/_types/_u_int.h values.
 */

#include <sys/cdefs.h>
#include <sys/_types/_u_int.h>

#ifndef _SHIM_HAS_UINT_T
#define _SHIM_HAS_UINT_T
typedef unsigned int    uint_t;
typedef volatile unsigned int   vuint_t;
typedef unsigned int    u_int;
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned long   u_long;
#endif

/*
 * mig port-flavour typedefs. In the real build these come from mig's
 * ctype mapping (std_types.defs: every *_move_send_t / *_consume_ref_t
 * is `ctype: mach_port_t`); no C header in the tree defines them.
 */
#ifndef _SHIM_MIG_PORT_TYPES
#define _SHIM_MIG_PORT_TYPES
#include <mach/port.h>
typedef mach_port_t     mach_port_move_receive_t;
typedef mach_port_t     mach_port_copy_send_t;
typedef mach_port_t     mach_port_make_send_t;
typedef mach_port_t     mach_port_move_send_t;
typedef mach_port_t     mach_port_make_send_once_t;
typedef mach_port_t     mach_port_move_send_once_t;
typedef mach_port_t     mach_port_receive_t;
typedef mach_port_t     mach_port_send_t;
typedef mach_port_t     mach_port_send_once_t;
typedef mach_port_t     mach_port_poly_t;
typedef mach_port_t     thread_act_consume_ref_t;
typedef mach_port_t     mem_entry_name_port_move_send_t;
typedef mach_port_t     semaphore_consume_ref_t;
typedef mach_port_t     eventlink_consume_ref_t;

/*
 * kern_types.h under MACH_KERNEL_PRIVATE typedefs `struct clock
 * *clock_t` (a Mach clock object). The bsd `clock_t` from
 * sys/_types/_clock_t.h is a different thing; osfmk files must not
 * see the bsd one. Pre-mark the bsd guard as taken so _clock_t.h
 * becomes a no-op whenever MACH_KERNEL_PRIVATE is on (this header is
 * force-included before any bsd header can define _CLOCK_T).
 */
#if defined(MACH_KERNEL_PRIVATE)
#ifndef _CLOCK_T
#define _CLOCK_T
#endif
#endif
#endif

/*
 * osfmk's kern/thread_call.h typedef is hidden from bsd consumers: bsd/net
 * headers use thread_call_t but the tree's own include order never provides
 * it (kern/thread.h exposes it only under MACH_KERNEL_PRIVATE, which must
 * stay off for bsd files). The SDK build gets it from clang modules
 * (USE_CLANG_TYPES); define the same opaque handle here.
 */
#ifndef _SHIM_THREAD_CALL_T
#define _SHIM_THREAD_CALL_T
typedef struct thread_call *thread_call_t;
#endif

#ifndef _SHIM_SCHED_PERFCONTROL_OPTIONS
#define _SHIM_SCHED_PERFCONTROL_OPTIONS
typedef uint64_t sched_perfcontrol_preferred_pset_options_t;
typedef sched_perfcontrol_preferred_pset_options_t sched_perfcontrol_preferred_cluster_options_t;
#endif

#endif
