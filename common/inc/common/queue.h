/*****************************************************************************
 * File:        queue.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_QUEUE_H
#define COMMON_QUEUE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "common/mem.h"
#include "common/pp.h"

#define QUEUE_DECL(TYPE, NAME)                                                \
    typedef struct queue_##NAME##_s {                                         \
        TYPE *data;                                                           \
        size_t head;                                                          \
        size_t size;                                                          \
        size_t cap;                                                           \
    } queue_##NAME##_t;

#define QUEUE_FRONT(Q) ((Q)->data + (Q)->head)
#define QUEUE_SIZE(Q)  ((Q)->size)

#define QUEUE_NEW(Q)                                                          \
    do {                                                                      \
        (Q)->data = NULL;                                                     \
        (Q)->head = 0;                                                        \
        (Q)->size = 0;                                                        \
        (Q)->cap  = 0;                                                        \
    } while (0)

#define QUEUE_FREE(Q) free((Q)->data);

#define QUEUE_PUSH(Q, VAL)                                                    \
    do {                                                                      \
        if ((Q)->size == (Q)->cap) {                                          \
            size_t _nc = (Q)->cap ? (Q)->cap * 2 : 4;                         \
            void *_nd  = checked_malloc(_nc * sizeof(*(Q)->data));            \
            if ((Q)->size > 0) {                                              \
                size_t _f = MIN((Q)->size, (Q)->cap - (Q)->head);             \
                memcpy(_nd, (Q)->data + (Q)->head, _f * sizeof(*(Q)->data));  \
                if ((Q)->size > _f)                                           \
                    memcpy((char *)_nd + _f * sizeof(*(Q)->data), (Q)->data,  \
                           ((Q)->size - _f) * sizeof(*(Q)->data));            \
            }                                                                 \
            free((Q)->data);                                                  \
            (Q)->data = _nd;                                                  \
            (Q)->head = 0;                                                    \
            (Q)->cap  = _nc;                                                  \
        }                                                                     \
        (Q)->data[((Q)->head + (Q)->size) % (Q)->cap] = *(VAL);               \
        (Q)->size++;                                                          \
    } while (0)

#define QUEUE_POP(Q)                                                          \
    do {                                                                      \
        (Q)->head = ((Q)->head + 1) % (Q)->cap;                               \
        (Q)->size--;                                                          \
    } while (0)

#endif
