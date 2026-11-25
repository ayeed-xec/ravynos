#ifndef _SYS_CALLOUT_SHIM_H_
#define _SYS_CALLOUT_SHIM_H_

#include <sys/types.h>

struct callout;
typedef struct callout *callout_handle_t;
typedef void (*callout_func_t)(void *);

struct callout_impl;
struct callout {
    struct callout_impl *c_impl;
    callout_func_t c_func;
    void *c_arg;
};

void callout_init(struct callout *c, int mpsafe);
void callout_init_mtx(struct callout *c, void *mtx, int flags);
int callout_stop(struct callout *c);
int callout_drain(struct callout *c);
int callout_reset(struct callout *c, int ticks, callout_func_t fn,
    void *arg);
int callout_schedule(struct callout *c, int ticks);
int callout_active(struct callout *c);

#endif
