#ifndef _SYS_RMAN_SHIM_H_
#define _SYS_RMAN_SHIM_H_

#include <sys/types.h>
#include <machine/resource.h>

struct rman;
typedef struct rman *rman_t;

uint64_t rman_get_start(struct resource *r);
uint64_t rman_get_size(struct resource *r);
int rman_get_flags(struct resource *r);
int rman_activate_resource(struct resource *r);
int rman_deactivate_resource(struct resource *r);
int rman_release_resource(struct resource *r);
struct resource *rman_reserve_resource(struct rman *rm, rman_res_t start,
    rman_res_t end, rman_res_t count, uint32_t flags, device_t child,
    device_t provider, void *softc);

#endif
