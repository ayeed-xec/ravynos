#ifndef _SYS_QUEUE_SHIM_H_
#define _SYS_QUEUE_SHIM_H_

#define TAILQ_HEAD(name, type) \
struct name { \
    struct type *tqh_first; \
    struct type **tqh_last; \
}

#define TAILQ_HEAD_INITIALIZER(head) \
    { NULL, &(head).tqh_first }

#define TAILQ_ENTRY(type) \
struct { \
    struct type *tqe_next; \
    struct type **tqe_prev; \
}

#define TAILQ_INIT(head) do { \
    (head)->tqh_first = NULL; \
    (head)->tqh_last = &(head)->tqh_first; \
} while (0)

#define TAILQ_EMPTY(head) ((head)->tqh_first == NULL)

#define TAILQ_FIRST(head) ((head)->tqh_first)

#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define TAILQ_INSERT_TAIL(head, elm, field) do { \
    (elm)->field.tqe_next = NULL; \
    (elm)->field.tqe_prev = (head)->tqh_last; \
    *(head)->tqh_last = (elm); \
    (head)->tqh_last = &(elm)->field.tqe_next; \
} while (0)

#define TAILQ_INSERT_HEAD(head, elm, field) do { \
    (elm)->field.tqe_next = (head)->tqh_first; \
    (elm)->field.tqe_prev = &(head)->tqh_first; \
    if ((head)->tqh_first != NULL) \
        (head)->tqh_first->field.tqe_prev = &(elm)->field.tqe_next; \
    else \
        (head)->tqh_last = &(elm)->field.tqe_next; \
    (head)->tqh_first = (elm); \
} while (0)

#define TAILQ_REMOVE(head, elm, field) do { \
    if ((elm)->field.tqe_next != NULL) \
        (elm)->field.tqe_next->field.tqe_prev = (elm)->field.tqe_prev; \
    else \
        (head)->tqh_last = (elm)->field.tqe_prev; \
    *(elm)->field.tqe_prev = (elm)->field.tqe_next; \
} while (0)

#define TAILQ_FOREACH(var, head, field) \
    for ((var) = (head)->tqh_first; \
        (var) != NULL; \
        (var) = (var)->field.tqe_next)

#define TAILQ_FOREACH_SAFE(var, head, field, tvar) \
    for ((var) = (head)->tqh_first; \
        (var) != NULL && ((tvar) = (var)->field.tqe_next, 1); \
        (var) = (tvar))

#define LIST_HEAD(name, type) \
struct name { \
    struct type *lh_first; \
}

#define LIST_HEAD_INITIALIZER(head) { NULL }

#define LIST_ENTRY(type) \
struct { \
    struct type *le_next; \
    struct type **le_prev; \
}

#define LIST_INIT(head) do { \
    (head)->lh_first = NULL; \
} while (0)

#define LIST_EMPTY(head) ((head)->lh_first == NULL)

#define LIST_FIRST(head) ((head)->lh_first)

#define LIST_NEXT(elm, field) ((elm)->field.le_next)

#define LIST_INSERT_HEAD(head, elm, field) do { \
    (elm)->field.le_next = (head)->lh_first; \
    if ((head)->lh_first != NULL) \
        (head)->lh_first->field.le_prev = &(elm)->field.le_next; \
    (head)->lh_first = (elm); \
    (elm)->field.le_prev = &(head)->lh_first; \
} while (0)

#define LIST_REMOVE(elm, field) do { \
    if ((elm)->field.le_next != NULL) \
        (elm)->field.le_next->field.le_prev = (elm)->field.le_prev; \
    *(elm)->field.le_prev = (elm)->field.le_next; \
} while (0)

#define LIST_FOREACH(var, head, field) \
    for ((var) = (head)->lh_first; \
        (var) != NULL; \
        (var) = (var)->field.le_next)

#define SLIST_HEAD(name, type) \
struct name { \
    struct type *slh_first; \
}

#define SLIST_ENTRY(type) \
struct { \
    struct type *sle_next; \
}

#define SLIST_INIT(head) do { \
    (head)->slh_first = NULL; \
} while (0)

#define SLIST_FIRST(head) ((head)->slh_first)

#define SLIST_EMPTY(head) ((head)->slh_first == NULL)

#define SLIST_INSERT_HEAD(head, elm, field) do { \
    (elm)->field.sle_next = (head)->slh_first; \
    (head)->slh_first = (elm); \
} while (0)

#define SLIST_REMOVE_HEAD(head, field) do { \
    (head)->slh_first = (head)->slh_first->field.sle_next; \
} while (0)

#define SLIST_FOREACH(var, head, field) \
    for ((var) = (head)->slh_first; \
        (var) != NULL; \
        (var) = (var)->field.sle_next)

#endif
