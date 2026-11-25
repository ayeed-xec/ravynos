#ifndef _CORE_ENTITLEMENTS_V2_API_H
#define _CORE_ENTITLEMENTS_V2_API_H
/*
 * V2 layout forwarder: amfi.h includes <CoreEntitlements/V2/API.h>,
 * but the xnu tarball ships the CoreEntitlements headers flat under
 * EXTERNAL_HEADERS/CoreEntitlements. Point at the real headers.
 */
#include <CoreEntitlements/CoreEntitlements.h>
#include <CoreEntitlements/Entitlements.h>
#include <CoreEntitlements/Errors.h>
#include <CoreEntitlements/Runtime.h>
#endif
