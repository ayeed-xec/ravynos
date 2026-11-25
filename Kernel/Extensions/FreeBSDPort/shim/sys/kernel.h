#ifndef _SYS_KERNEL_SHIM_H_
#define _SYS_KERNEL_SHIM_H_

#include <sys/param.h>
#include <sys/malloc.h>

#define SI_SUB_DRIVERS 0x2400000
#define SI_ORDER_FIRST 0x1
#define SI_ORDER_MIDDLE 0x6
#define SI_ORDER_LAST 0xE

#define __TUNABLE_NAME(x, y) x ## y
#define __TUNABLE_UNIQ(x) __TUNABLE_NAME(__shim_tunable_shim_, x)

#define TUNABLE_INT(path, var) \
    static void *__TUNABLE_UNIQ(__LINE__)[] = { (void *)(path), (void *)(var) }

int TUNABLE_INT_FETCH(const char *path, int *val);

#endif
