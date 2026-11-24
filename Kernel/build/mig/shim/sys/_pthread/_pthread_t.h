#ifndef _MIG_SHIM_PTHREAD_T_H
#define _MIG_SHIM_PTHREAD_T_H
typedef struct _opaque_pthread_t *__darwin_pthread_t;
typedef __darwin_pthread_t pthread_t;
#endif
