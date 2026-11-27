// SPDX-License-Identifier: APSL-2.0
/* Minimal Mach header shim for building Apple's migcom on Linux.
 * Mirrors xnu's osfmk/mach/ndr.h NDR_record_t (SPDX:APSL-2.0); the
 * struct layout must stay byte-for-byte identical (sizeof is baked
 * into generated headers). */
#ifndef _MACH_NDR_H_SHIM_
#define _MACH_NDR_H_SHIM_

typedef struct {
        unsigned char   mig_vers;
        unsigned char   if_vers;
        unsigned char   reserved1;
        unsigned char   mig_encoding;
        unsigned char   int_rep;
        unsigned char   char_rep;
        unsigned char   float_rep;
        unsigned char   reserved2;
} NDR_record_t;

#define NDR_PROTOCOL_2_0                0
#define NDR_INT_BIG_ENDIAN              0
#define NDR_INT_LITTLE_ENDIAN           1
#define NDR_FLOAT_IEEE                  0
#define NDR_FLOAT_VAX                   1
#define NDR_FLOAT_CRAY                  2
#define NDR_FLOAT_IBM                   3
#define NDR_CHAR_ASCII                  0

#endif /* _MACH_NDR_H_SHIM_ */
