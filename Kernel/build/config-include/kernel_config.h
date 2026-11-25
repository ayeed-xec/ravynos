#ifndef _KERNEL_CONFIG_H
#define _KERNEL_CONFIG_H

#define CONFIG_COALITIONS 1
#define CONFIG_COALITIONS_TASKTRANSFER 1
#define CONFIG_SCHED_TIMESHARE_CORE 1
#define CONFIG_SCHED_PROTO 1
#define CONFIG_SCHED_SMT 1
#define CONFIG_SCHED_AUTO_JOIN 1
#define CONFIG_THREAD_GROUPS 1
#define CONFIG_PREADOPT_TG 1
#define CONFIG_DTRACE 1
#define CONFIG_CPU_COUNTERS 1
#define CONFIG_FREEZE 1
#define CONFIG_IOSCHED 1
#define CONFIG_MEMORYSTATUS 1
#define CONFIG_TASKWATCH 1
#define CONFIG_PROC_RESOURCE_LIMITS 1
#define CONFIG_MACF 1

/*
 * MASTER option: max threads (config/MASTER:658, the medium/large/xlarge
 * value). Used as thread_max/task_threadmax in kern/thread.c; must stay in
 * sync with bsd/conf/MASTER per the comment there.
 */
#ifndef CONFIG_THREAD_MAX
#define CONFIG_THREAD_MAX 2560
#endif

/*
 * MASTER option: deferred VM reclaim (config/MASTER:373), default-on for
 * x86_64 desktop kernels. Gates kern/task.c suspend hooks and
 * vm/vm_reclaim.c.
 */
#ifndef CONFIG_DEFERRED_RECLAIM
#define CONFIG_DEFERRED_RECLAIM 1
#endif

/*
 * MASTER option: max tasks (config/MASTER:662, the medium/large/xlarge
 * value, paired with CONFIG_THREAD_MAX above). thread_group.h sizes
 * THREAD_GROUP_MAX from it.
 */
#ifndef CONFIG_TASK_MAX
#define CONFIG_TASK_MAX 1024
#endif

/*
 * MASTER options: ipc table limits (config/MASTER:291-294, the
 * medium/large/xlarge values, matching CONFIG_TASK_MAX above).
 * Used by ipc_entry.c and ipc_port.c table sizing.
 */
#ifndef CONFIG_IPC_TABLE_ENTRIES_SIZE_MAX
#define CONFIG_IPC_TABLE_ENTRIES_SIZE_MAX 7340032
#endif

#ifndef CONFIG_IPC_TABLE_REQUEST_SIZE_MAX
#define CONFIG_IPC_TABLE_REQUEST_SIZE_MAX 2097152
#endif

/*
 * MASTER option: kernel-event hash sizes (config/MASTER:185, the
 * medium/large/xlarge value). Sizes the kqueue/knote hash in kern_event.c.
 */
#ifndef CONFIG_KN_HASHSIZE
#define CONFIG_KN_HASHSIZE 64
#endif

/*
 * Deployment target: the x86_64 desktop build (config/MASTER.x86_64 with
 * PLATFORM_FOR_CONFIG=MacOSX, makedefs/MakeInc.def:133). Gates the
 * progress-meter console path and task policy fields.
 */
#ifndef XNU_TARGET_OS_OSX
#define XNU_TARGET_OS_OSX 1
#endif

/*
 * bsd private driver declarations (dev/busvar.h pseudo_inits etc.)
 * come from DRIVER_PRIVATE in the real build.
 */
#ifndef DRIVER_PRIVATE
#define DRIVER_PRIVATE 1
#endif

/*
 * Zone-map sizing: the real build derives this per-platform in the
 * MASTER/MakeInc.def flow. Syntax-check stand-in value only; revisit
 * when the kernel links.
 */
#ifndef CONFIG_ZONE_MAP_MIN
#define CONFIG_ZONE_MAP_MIN (512UL << 20)
#endif

/*
 * MASTER option: Multipath TCP (config/MASTER:75). The real build
 * passes -DMPTCP=1 to every bsd file that needs it. Default off:
 * no BSD code in this tree exercises it, and a 1 default hid real
 * missing-definition errors behind dead code.
 */
#ifndef MPTCP
#define MPTCP 0
#endif

/*
 * MASTER option: BSD subsystem on top of Mach (config/MASTER:651).
 */
#ifndef MACH_BSD
#define MACH_BSD 1
#endif

/*
 * MASTER option: kperf tracing (config/MASTER:631). kern/thread.h gates
 * the per-thread kperf_ast fields on it and kern/ast.c writes them.
 */
#ifndef KPERF
#define KPERF 1
#endif

/*
 * MASTER option: traffic management support (config/MASTER:71). tcp_var.h
 * gates CLEAR_IAJ_STATE and the iaj_rcv_ts field on it.
 */
#ifndef TRAFFIC_MGT
#define TRAFFIC_MGT 1
#endif

/*
 * MASTER option: U32 syscall arg munging (config/MASTER:743). Required on
 * x86_64; bsd_i386.c errors out without it.
 */#ifndef CONFIG_REQUIRES_U32_MUNGING
#define CONFIG_REQUIRES_U32_MUNGING 1
#endif

/*
 * MASTER option: DEBUG is off for release kernels (config/MASTER:646 is a
 * commented-out option); x86_64 pmap.c initializes no_shared_cr3 from it.
 */
#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef DEVELOPMENT
#define DEVELOPMENT 1
#endif

/*
 * MASTER option (debug/development builds): zone allocation logging.
 * zalloc_internal.h gates the z_btlog members on it, matching zalloc.c's
 * ZALLOC_ENABLE_LOGGING use sites.
 */
#if DEBUG || DEVELOPMENT
#define ZONE_ENABLE_LOGGING 1
#endif

/*
 * MASTER option: CONFIG_MAX_CLUSTERS=8 (config/MASTER:216, large/xlarge
 * cluster size); bsd/sys/ubc_internal.h sizes the upl cluster arrays with it.
 */
#ifndef CONFIG_MAX_CLUSTERS
#define CONFIG_MAX_CLUSTERS 8
#endif

/*
 * The kernel is the sanctioned in-process consumer of the CoreEntitlements
 * private API (libkern/libkern/amfi/amfi.h includes CoreEntitlementsPriv.h);
 * the guard exists for out-of-tree adopters.
 */
#ifndef CORE_ENTITLEMENTS_I_KNOW_WHAT_IM_DOING
#define CORE_ENTITLEMENTS_I_KNOW_WHAT_IM_DOING 1
#endif

/*
 * ipc_port watchport element API and IPC importance propagation.
 */
#ifndef IMPORTANCE_INHERITANCE
#define IMPORTANCE_INHERITANCE 1
#endif

/*
 * osfmk/kern/kalloc.h relies on Apple-clang-only builtins for its
 * kalloc_type signature checks. Upstream clang has neither; report every
 * granule present so checks stay permissive and kalloc_type calls route
 * to the generic zone until the real builtins are available.
 */
#if !__has_builtin(__builtin_xnu_type_summary)
#define __builtin_xnu_type_summary(type) KT_SUMMARY_MASK_ALL_GRANULES
#endif
#if !__has_builtin(__builtin_xnu_type_signature)
#define __builtin_xnu_type_signature(type) ""
#endif
#if !__has_builtin(__builtin_xnu_types_compatible)
#define __builtin_xnu_types_compatible(a, b) 1
#endif

#endif
