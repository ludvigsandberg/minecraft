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
#include <assert.h>

#include "common/mem.h"
#include "common/pp.h"

#define QUEUE_DECL(TYPE, NAME)                                                \
    typedef struct queue_##NAME##_s {                                         \
        TYPE *data;                                                           \
        size_t head;                                                          \
        size_t size;                                                          \
        size_t cap;                                                           \
    } queue_##NAME##_t;                                                       \
                                                                              \
    TYPE *queue_##NAME##_front(queue_##NAME##_t *q);                          \
    size_t queue_##NAME##_size(const queue_##NAME##_t *q);                    \
                                                                              \
    void queue_##NAME##_new(queue_##NAME##_t *q);                             \
    void queue_##NAME##_free(queue_##NAME##_t *q);                            \
    void queue_##NAME##_push(queue_##NAME##_t *q, const TYPE *val);           \
    void queue_##NAME##_pop(queue_##NAME##_t *q);

#define QUEUE_DEF(TYPE, NAME)                                                 \
    TYPE *queue_##NAME##_front(queue_##NAME##_t *q) {                         \
        assert(q);                                                            \
        assert(q->size > 0);                                                  \
        return q->data + q->head;                                             \
    }                                                                         \
                                                                              \
    size_t queue_##NAME##_size(const queue_##NAME##_t *q) {                   \
        assert(q);                                                            \
        return q->size;                                                       \
    }                                                                         \
                                                                              \
    void queue_##NAME##_new(queue_##NAME##_t *q) {                            \
        assert(q);                                                            \
        q->data = NULL;                                                       \
        q->head = 0;                                                          \
        q->size = 0;                                                          \
        q->cap  = 0;                                                          \
    }                                                                         \
                                                                              \
    void queue_##NAME##_free(queue_##NAME##_t *q) {                           \
        assert(q);                                                            \
        free(q->data);                                                        \
    }                                                                         \
                                                                              \
    void queue_##NAME##_push(queue_##NAME##_t *q, const TYPE *val) {          \
        TYPE *new_data;                                                       \
        size_t new_cap;                                                       \
        size_t first;                                                         \
        size_t tail;                                                          \
        assert(q);                                                            \
        assert(val);                                                          \
        if (q->size == q->cap) {                                              \
            new_cap  = q->cap ? q->cap * 2 : 4;                               \
            new_data = checked_malloc(new_cap * sizeof(TYPE));                \
            if (q->size > 0) {                                                \
                first = MIN(q->size, q->cap - q->head);                       \
                memcpy(new_data, q->data + q->head, first * sizeof(TYPE));    \
                if (q->size > first) {                                        \
                    memcpy(new_data + first, q->data,                         \
                           (q->size - first) * sizeof(TYPE));                 \
                }                                                             \
            }                                                                 \
            free(q->data);                                                    \
            q->data = new_data;                                               \
            q->head = 0;                                                      \
            q->cap  = new_cap;                                                \
        }                                                                     \
        tail          = (q->head + q->size) % q->cap;                         \
        q->data[tail] = *val;                                                 \
        q->size++;                                                            \
    }                                                                         \
                                                                              \
    void queue_##NAME##_pop(queue_##NAME##_t *q) {                            \
        assert(q);                                                            \
        assert(q->size > 0);                                                  \
        q->head = (q->head + 1) % q->cap;                                     \
        q->size--;                                                            \
    }

#endif
