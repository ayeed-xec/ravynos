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
 * passes -DMPTCP=1 to every bsd file that needs it.
 */
#ifndef MPTCP
#define MPTCP 1
#endif

/*
 * MASTER option: BSD subsystem on top of Mach (config/MASTER:651).
 */
#ifndef MACH_BSD
#define MACH_BSD 1
#endif

#endif
