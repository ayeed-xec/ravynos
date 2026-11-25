#ifndef _SHIM_TRUSTCACHE_API_H
#define _SHIM_TRUSTCACHE_API_H

/*
 * Clean-room stand-in for Apple's rust-derived libTrustCache kernel
 * headers, which are not shipped in the public xnu tarball. Only the
 * opaque types xnu's own headers reference are declared here; the
 * real runtime lives in Apple's TrustCache repo (not vendored, GPL
 * concerns do not apply but the sources are not public as headers).
 */

#include <stdint.h>
#include <uuid.h>

enum {
	kUUIDSize = 16,
};

typedef struct TrustCache TrustCache_t;
typedef struct TrustCacheRuntime TrustCacheRuntime_t;
typedef struct TrustCacheMutableRuntime TrustCacheMutableRuntime_t;

typedef struct TrustCacheQueryToken {
	const TrustCache_t *trustCache;
	uint64_t token;
} TrustCacheQueryToken_t;

typedef struct TrustCacheObjectID {
	uint64_t id;
} TrustCacheObjectID_t;

enum {
	kTCEntryHashSize = 20,
};

typedef enum {
	kTCTypeStatic = 0,
	kTCTypeDynamic = 1,
} TCType_t;

typedef enum {
	kTCQueryTypeCDHash = 0,
} TCQueryType_t;

typedef uint64_t TCCapabilities_t;

#endif
