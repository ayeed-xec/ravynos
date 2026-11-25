#ifndef _SYS_SYSTM_SHIM_H_
#define _SYS_SYSTM_SHIM_H_

#include <sys/types.h>
#include <sys/callout.h>
#include <libkern/libkern.h>

extern int ticks;
extern int time_uptime;
extern int time_second;

struct mtx;

int tsleep(void *ident, int priority, const char *wmesg, int timo);

#define KASSERT(e, msg) do { \
    if (!(e)) \
        panic msg; \
} while (0)

void shim_report_unimplemented(const char *what);

#endif
