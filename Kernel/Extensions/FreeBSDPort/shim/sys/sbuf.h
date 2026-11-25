#ifndef _SYS_SBUF_SHIM_H_
#define _SYS_SBUF_SHIM_H_

#include <sys/types.h>

struct sbuf {
    char *s_buf;
    int s_size;
    int s_len;
};

typedef struct sbuf sbuf_t;

#define SBUF_FIXEDLEN 0
#define SBUF_AUTOEXTEND 1

int sbuf_new(struct sbuf *s, char *buf, int length, int flags);
int sbuf_delete(struct sbuf *s);
int sbuf_cat(struct sbuf *s, const char *str);
int sbuf_printf(struct sbuf *s, const char *fmt, ...);
int sbuf_finish(struct sbuf *s);
char *sbuf_data(struct sbuf *s);
int sbuf_len(struct sbuf *s);
int sbuf_overflowed(struct sbuf *s);
int sbuf_clear(struct sbuf *s);

#endif
