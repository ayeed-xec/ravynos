#ifndef	_resource_notify_server_
#define	_resource_notify_server_

/* Module resource_notify */

#include <string.h>
#include <mach/ndr.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
	
/* BEGIN MIG_STRNCPY_ZEROFILL CODE */

#if defined(__has_include)
#if __has_include(<mach/mig_strncpy_zerofill_support.h>)
#ifndef USING_MIG_STRNCPY_ZEROFILL
#define USING_MIG_STRNCPY_ZEROFILL
#endif
#ifndef __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#define __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
	extern int mig_strncpy_zerofill(char *dest, const char *src, int len) __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif /* __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__ */
#endif /* __has_include(<mach/mig_strncpy_zerofill_support.h>) */
#endif /* __has_include */
	
/* END MIG_STRNCPY_ZEROFILL CODE */


#ifdef AUTOTEST
#ifndef FUNCTION_PTR_T
#define FUNCTION_PTR_T
typedef void (*function_ptr_t)(mach_port_t, char *, mach_msg_type_number_t);
typedef struct {
        char            *name;
        function_ptr_t  function;
} function_table_entry;
typedef function_table_entry   *function_table_t;
#endif /* FUNCTION_PTR_T */
#endif /* AUTOTEST */

#ifndef	resource_notify_MSG_COUNT
#define	resource_notify_MSG_COUNT	6
#endif	/* resource_notify_MSG_COUNT */

#include <Availability.h>
#include <mach/std_types.h>
#include <mach/mig.h>
#include <ipc/ipc_voucher.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_tt.h>
#include <kern/ipc_host.h>
#include <kern/ledger.h>
#include <kern/processor.h>
#include <kern/sync_sema.h>
#include <ipc/ipc_eventlink.h>
#include <vm/memory_object.h>
#include <vm/vm_map.h>
#include <kern/ipc_mig.h>
#include <kern/task_ident.h>
#include <kern/kern_cdata.h>
#include <mach/mig.h>
#include <mach/mach_types.h>
#include <mach/mach_types.h>
#include <mach/resource_monitors.h>
#include <mach/clock_types.h>

#ifdef __BeforeMigServerHeader
__BeforeMigServerHeader
#endif /* __BeforeMigServerHeader */


/* SimpleRoutine cpu_usage_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_cpu_usage_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	posix_path_t killed_proc_path,
	mach_timespec_t timestamp,
	int64_t observed_cpu_nsecs,
	int64_t observation_nsecs,
	int64_t cpu_nsecs_allowed,
	int64_t limit_window_nsecs,
	resource_notify_flags_t flags
);

/* SimpleRoutine cpu_wakes_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_cpu_wakes_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	posix_path_t killed_proc_path,
	mach_timespec_t timestamp,
	int64_t observed_cpu_wakes,
	int64_t observation_nsecs,
	int64_t cpu_wakes_allowed,
	int64_t limit_window_nsecs,
	resource_notify_flags_t flags
);

/* SimpleRoutine disk_writes_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_disk_writes_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	posix_path_t killed_proc_path,
	mach_timespec_t timestamp,
	int64_t observed_bytes_dirtied,
	int64_t observation_nsecs,
	int64_t bytes_dirtied_allowed,
	int64_t limit_window_nsecs,
	resource_notify_flags_t flags
);

/* SimpleRoutine port_space_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_port_space_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	mach_timespec_t timestamp,
	int64_t observed_ports,
	int64_t ports_allowed,
	mach_port_t fatal_port,
	resource_notify_flags_t flags
);

/* SimpleRoutine file_descriptors_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_file_descriptors_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	mach_timespec_t timestamp,
	int64_t observed_filedesc,
	int64_t filedesc_allowed,
	mach_port_t fatal_port,
	resource_notify_flags_t flags
);

/* SimpleRoutine kqworkloops_violation */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t receive_kqworkloops_violation
(
	mach_port_t receiver,
	proc_name_t procname,
	int pid,
	mach_timespec_t timestamp,
	int64_t observed_kqworkloops,
	int64_t kqworkloops_allowed,
	mach_port_t fatal_port,
	resource_notify_flags_t flags
);

#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
boolean_t resource_notify_server(
		mach_msg_header_t *InHeadP,
		mach_msg_header_t *OutHeadP);

#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
mig_routine_t resource_notify_server_routine(
		mach_msg_header_t *InHeadP);


/* Description of this subsystem, for use in direct RPC */
extern const struct receive_resource_notify_subsystem {
	mig_server_routine_t	server;	/* Server routine */
	mach_msg_id_t	start;	/* Min routine number */
	mach_msg_id_t	end;	/* Max routine number + 1 */
	unsigned int	maxsize;	/* Max msg size */
	vm_address_t	reserved;	/* Reserved */
	struct routine_descriptor	/*Array of routine descriptors */
		routine[6];
} receive_resource_notify_subsystem;

/* typedefs for all requests */

#ifndef __Request__resource_notify_subsystem__defined
#define __Request__resource_notify_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		posix_path_t killed_proc_path;
		mach_timespec_t timestamp;
		int64_t observed_cpu_nsecs;
		int64_t observation_nsecs;
		int64_t cpu_nsecs_allowed;
		int64_t limit_window_nsecs;
		resource_notify_flags_t flags;
	} __Request__cpu_usage_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		posix_path_t killed_proc_path;
		mach_timespec_t timestamp;
		int64_t observed_cpu_wakes;
		int64_t observation_nsecs;
		int64_t cpu_wakes_allowed;
		int64_t limit_window_nsecs;
		resource_notify_flags_t flags;
	} __Request__cpu_wakes_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		posix_path_t killed_proc_path;
		mach_timespec_t timestamp;
		int64_t observed_bytes_dirtied;
		int64_t observation_nsecs;
		int64_t bytes_dirtied_allowed;
		int64_t limit_window_nsecs;
		resource_notify_flags_t flags;
	} __Request__disk_writes_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_port_descriptor_t fatal_port;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		mach_timespec_t timestamp;
		int64_t observed_ports;
		int64_t ports_allowed;
		resource_notify_flags_t flags;
	} __Request__port_space_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_port_descriptor_t fatal_port;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		mach_timespec_t timestamp;
		int64_t observed_filedesc;
		int64_t filedesc_allowed;
		resource_notify_flags_t flags;
	} __Request__file_descriptors_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		/* start of the kernel processed data */
		mach_msg_body_t msgh_body;
		mach_msg_port_descriptor_t fatal_port;
		/* end of the kernel processed data */
		NDR_record_t NDR;
		proc_name_t procname;
		char procnamePad[3];
		int pid;
		mach_timespec_t timestamp;
		int64_t observed_kqworkloops;
		int64_t kqworkloops_allowed;
		resource_notify_flags_t flags;
	} __Request__kqworkloops_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Request__resource_notify_subsystem__defined */


/* union of all requests */

#ifndef __RequestUnion__receive_resource_notify_subsystem__defined
#define __RequestUnion__receive_resource_notify_subsystem__defined
union __RequestUnion__receive_resource_notify_subsystem {
	__Request__cpu_usage_violation_t Request_cpu_usage_violation;
	__Request__cpu_wakes_violation_t Request_cpu_wakes_violation;
	__Request__disk_writes_violation_t Request_disk_writes_violation;
	__Request__port_space_violation_t Request_port_space_violation;
	__Request__file_descriptors_violation_t Request_file_descriptors_violation;
	__Request__kqworkloops_violation_t Request_kqworkloops_violation;
};
#endif /* __RequestUnion__receive_resource_notify_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__resource_notify_subsystem__defined
#define __Reply__resource_notify_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__cpu_usage_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__cpu_wakes_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__disk_writes_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__port_space_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__file_descriptors_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
	} __Reply__kqworkloops_violation_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Reply__resource_notify_subsystem__defined */


/* union of all replies */

#ifndef __ReplyUnion__receive_resource_notify_subsystem__defined
#define __ReplyUnion__receive_resource_notify_subsystem__defined
union __ReplyUnion__receive_resource_notify_subsystem {
	__Reply__cpu_usage_violation_t Reply_cpu_usage_violation;
	__Reply__cpu_wakes_violation_t Reply_cpu_wakes_violation;
	__Reply__disk_writes_violation_t Reply_disk_writes_violation;
	__Reply__port_space_violation_t Reply_port_space_violation;
	__Reply__file_descriptors_violation_t Reply_file_descriptors_violation;
	__Reply__kqworkloops_violation_t Reply_kqworkloops_violation;
};
#endif /* __RequestUnion__receive_resource_notify_subsystem__defined */

#ifndef subsystem_to_name_map_resource_notify
#define subsystem_to_name_map_resource_notify \
    { "cpu_usage_violation", 827800 },\
    { "cpu_wakes_violation", 827801 },\
    { "disk_writes_violation", 827802 },\
    { "port_space_violation", 827803 },\
    { "file_descriptors_violation", 827804 },\
    { "kqworkloops_violation", 827805 }
#endif

#ifdef __AfterMigServerHeader
__AfterMigServerHeader
#endif /* __AfterMigServerHeader */

#endif	 /* _resource_notify_server_ */
