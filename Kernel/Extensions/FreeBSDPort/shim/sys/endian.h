#ifndef _SYS_ENDIAN_SHIM_H_
#define _SYS_ENDIAN_SHIM_H_

#include <sys/types.h>

#define __bswap16(x) __builtin_bswap16(x)
#define __bswap32(x) __builtin_bswap32(x)
#define __bswap64(x) __builtin_bswap64(x)

static inline uint16_t
le16toh(uint16_t v)
{
    return (v);
}

static inline uint32_t
le32toh(uint32_t v)
{
    return (v);
}

static inline uint64_t
le64toh(uint64_t v)
{
    return (v);
}

static inline uint16_t
htole16(uint16_t v)
{
    return (v);
}

static inline uint32_t
htole32(uint32_t v)
{
    return (v);
}

static inline uint64_t
htole64(uint64_t v)
{
    return (v);
}

static inline uint16_t
be16toh(uint16_t v)
{
    return (__bswap16(v));
}

static inline uint32_t
be32toh(uint32_t v)
{
    return (__bswap32(v));
}

static inline uint16_t
htobe16(uint16_t v)
{
    return (__bswap16(v));
}

static inline uint32_t
htobe32(uint32_t v)
{
    return (__bswap32(v));
}

#define __htole16(v) htole16(v)
#define __htole32(v) htole32(v)
#define __htole64(v) htole64(v)
#define __le16toh(v) le16toh(v)
#define __le32toh(v) le32toh(v)
#define __le64toh(v) le64toh(v)
#define __htobe16(v) htobe16(v)
#define __htobe32(v) htobe32(v)
#define __be16toh(v) be16toh(v)
#define __be32toh(v) be32toh(v)

#endif
