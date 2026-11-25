#ifndef _MACHINE_CPU_SHIM_H_
#define _MACHINE_CPU_SHIM_H_

#define cpu_spinwait() __asm volatile("pause")

static inline void
cpu_pause(void)
{
    __asm volatile("pause");
}

#endif
