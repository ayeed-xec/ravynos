#ifndef _SYS_TASKQUEUE_SHIM_H_
#define _SYS_TASKQUEUE_SHIM_H_

#include <sys/queue.h>
#include <sys/mutex.h>

struct task {
    TAILQ_ENTRY(task) ta_link;
    int ta_pending;
    void (*ta_func)(void *context, int pending);
    void *ta_context;
};

struct taskqueue;
typedef struct taskqueue *taskqueue_t;

typedef void (*taskqueue_enqueue_fn)(void *context);

#define TASK_INIT(task, prio, func, context) do { \
    (task)->ta_pending = 0; \
    (task)->ta_func = (func); \
    (task)->ta_context = (context); \
} while (0)

#define PI_NET 18
#define PI_DISK 17

struct taskqueue *taskqueue_create(const char *name, int mflags,
    taskqueue_enqueue_fn enqueue, void *context);
void taskqueue_free(struct taskqueue *queue);
int taskqueue_enqueue(struct taskqueue *queue, struct task *task);
void taskqueue_drain(struct taskqueue *queue, struct task *task);
int taskqueue_start_threads(struct taskqueue **queue, int count, int pri,
    const char *name, ...);
void taskqueue_block(struct taskqueue *queue);
void taskqueue_unblock(struct taskqueue *queue);

extern struct taskqueue *taskqueue_fast;
extern void taskqueue_thread_enqueue(void *context);
extern struct taskqueue *taskqueue_thread;
extern struct taskqueue *taskqueue_swi;

#endif
