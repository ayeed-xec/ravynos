#ifndef _MACHINE_ATOMIC_SHIM_H_
#define _MACHINE_ATOMIC_SHIM_H_

#include <sys/types.h>

static inline void
mb(void)
{
    __asm volatile("mfence" ::: "memory");
}

static inline void
rmb(void)
{
    __asm volatile("lfence" ::: "memory");
}

static inline void
wmb(void)
{
    __asm volatile("sfence" ::: "memory");
}

static inline void
atomic_thread_fence_seq_cst(void)
{
    __asm volatile("mfence" ::: "memory");
}

#define atomic_add_32(p, v) __sync_fetch_and_add((p), (v))
#define atomic_sub_32(p, v) __sync_fetch_and_sub((p), (v))
#define atomic_add_int(p, v) __sync_fetch_and_add((volatile int *)(p), (v))
#define atomic_subtract_int(p, v) __sync_fetch_and_sub((volatile int *)(p), \
    (v))
#define atomic_readandclear_int(p) __sync_fetch_and_and((volatile int *)(p), 0)
#define atomic_cmpset_int(p, o, n) __sync_bool_compare_and_swap( \
    (volatile int *)(p), (o), (n))
#define atomic_cmpset_32(p, o, n) __sync_bool_compare_and_swap( \
    (volatile uint32_t *)(p), (o), (n))
#define atomic_cmpset_64(p, o, n) __sync_bool_compare_and_swap( \
    (volatile uint64_t *)(p), (o), (n))
#define atomic_load_acq_int(p) (*(volatile int *)(p))
#define atomic_store_rel_int(p, v) do { \
    *(volatile int *)(p) = (v); \
} while (0)
#define atomic_load_acq_32(p) (*(volatile uint32_t *)(p))
#define atomic_store_rel_32(p, v) do { \
    *(volatile uint32_t *)(p) = (v); \
} while (0)

#endif
