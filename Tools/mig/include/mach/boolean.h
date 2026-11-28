// SPDX-License-Identifier: APSL-2.0
/* Minimal Mach header shim for building Apple's migcom on Linux.
 * Mirrors the definitions migcom actually uses from xnu's
 * osfmk/mach/boolean.h (SPDX:APSL-2.0). */
#ifndef _MACH_BOOLEAN_H_SHIM_
#define _MACH_BOOLEAN_H_SHIM_

typedef unsigned int boolean_t;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* _MACH_BOOLEAN_H_SHIM_ */
