#ifndef _MACHINE_VMPARAM_SHIM_H_
#define _MACHINE_VMPARAM_SHIM_H_

#include <sys/types.h>

typedef uint64_t vm_paddr_t;
typedef uint64_t vm_paddr_t_32;

#define VM_PAGE_SIZE 4096

extern unsigned long physmem;

#endif
