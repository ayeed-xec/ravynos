#ifndef _MIG_SHIM_MACHINE_TYPES_H
#define _MIG_SHIM_MACHINE_TYPES_H
#include <sys/_types/_int8_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>
#include <sys/_types/_u_int8_t.h>
#include <sys/_types/_u_int16_t.h>
#include <sys/_types/_u_int32_t.h>
#include <sys/_types/_u_int64_t.h>
#include <sys/_types/_intptr_t.h>
typedef int integer_t;
typedef unsigned int natural_t;
typedef unsigned long vm_offset_t;
typedef unsigned long vm_address_t;
typedef unsigned long vm_size_t;
typedef natural_t mach_msg_topology_t;
typedef natural_t mach_msg_right_t;
typedef u_int64_t user_addr_t;
typedef u_int64_t user_size_t;
typedef int64_t user_ssize_t;
typedef int64_t user_off_t;
typedef u_int64_t user_addr64_t;
#endif
