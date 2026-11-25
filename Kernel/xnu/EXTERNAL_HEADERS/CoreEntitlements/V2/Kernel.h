#ifndef _CORE_ENTITLEMENTS_V2_KERNEL_H
#define _CORE_ENTITLEMENTS_V2_KERNEL_H

/*
 * V2 layout forwarder: amfi.h includes <CoreEntitlements/V2/Kernel.h>,
 * but the xnu tarball ships the CoreEntitlements headers flat under
 * EXTERNAL_HEADERS/CoreEntitlements and omits the V2 kernel API.
 * CEContext_t and CEKernelAPI_t are opaque there; declare them so
 * amfi.h's interface typedefs compile.
 */

#include <CoreEntitlements/CoreEntitlements.h>
#include <CoreEntitlements/CoreEntitlementsPriv.h>

typedef const struct CEContext *CEContext_t;

typedef struct CEKernelAPI CEKernelAPI_t;

#endif
