#ifndef _SYS_RANDOM_SHIM_H_
#define _SYS_RANDOM_SHIM_H_

#include <sys/types.h>

int read_random(void *buf, int count);
void arc4rand(void *buf, unsigned int len, int reseed);
uint32_t arc4random(void);

#endif
