#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Fixed-size ring buffer. */
struct ring_buffer {
    size_t head;
    size_t tail;
    size_t size;
    size_t capacity; /* Capacity must be a power of two! */
};

#define RING_BUFFER_PUSH(QUEUE, ELEMS, ITEM)                                  \
    do {                                                                      \
        size_t _mask = (QUEUE).capacity - 1;                                  \
        assert(((QUEUE).capacity & _mask) == 0);                              \
        assert((QUEUE).size < (QUEUE).capacity);                              \
                                                                              \
        (ELEMS)[(QUEUE).tail] = (ITEM);                                       \
        (QUEUE).tail          = ((QUEUE).tail + 1) & _mask;                   \
        (QUEUE).size++;                                                       \
    } while (0)

#define RING_BUFFER_POP(QUEUE, ELEMS, OUT_ITEM)                               \
    do {                                                                      \
        size_t _mask = (QUEUE).capacity - 1;                                  \
        assert(((QUEUE).capacity & _mask) == 0);                              \
        assert((QUEUE).size > 0);                                             \
                                                                              \
        (OUT_ITEM)   = (ELEMS)[(QUEUE).head];                                 \
        (QUEUE).head = ((QUEUE).head + 1) & _mask;                            \
        (QUEUE).size--;                                                       \
    } while (0)

#define RING_BUFFER_PUSH_N(QUEUE, ELEMS, N, SRC)                              \
    do {                                                                      \
        size_t _n   = (N);                                                    \
        size_t _cap = (QUEUE).capacity;                                       \
        assert((_cap & (_cap - 1)) == 0);                                     \
        assert(_cap > 0);                                                     \
        assert((QUEUE).size + _n <= _cap);                                    \
                                                                              \
        if (_n > 0) {                                                         \
            size_t _mask        = _cap - 1;                                   \
            size_t _tail        = (QUEUE).tail & _mask;                       \
            size_t _avail       = _cap - _tail;                               \
            size_t _first_chunk = (_avail < _n) ? _avail : _n;                \
                                                                              \
            memcpy((ELEMS) + _tail, (SRC), _first_chunk * sizeof(*(ELEMS)));  \
                                                                              \
            if (_n > _first_chunk) {                                          \
                memcpy((ELEMS),                                               \
                       ((const char *)(SRC)) +                                \
                           (_first_chunk * sizeof(*(ELEMS))),                 \
                       (_n - _first_chunk) * sizeof(*(ELEMS)));               \
            }                                                                 \
                                                                              \
            (QUEUE).tail = (_tail + _n) & _mask;                              \
            (QUEUE).size += _n;                                               \
        }                                                                     \
    } while (0)

#define RING_BUFFER_POP_N(QUEUE, ELEMS, N, DST)                               \
    do {                                                                      \
        size_t _n   = (N);                                                    \
        size_t _cap = (QUEUE).capacity;                                       \
        assert((_cap & (_cap - 1)) == 0);                                     \
        assert(_cap > 0);                                                     \
        assert(_n <= (QUEUE).size);                                           \
                                                                              \
        if (_n > 0) {                                                         \
            size_t _mask        = _cap - 1;                                   \
            size_t _head        = (QUEUE).head & _mask;                       \
            size_t _avail       = _cap - _head;                               \
            size_t _first_chunk = (_avail < _n) ? _avail : _n;                \
                                                                              \
            if ((DST) != NULL) {                                              \
                memcpy((DST), (ELEMS) + _head,                                \
                       _first_chunk * sizeof(*(ELEMS)));                      \
                                                                              \
                if (_n > _first_chunk) {                                      \
                    memcpy(((char *)(DST)) +                                  \
                               (_first_chunk * sizeof(*(ELEMS))),             \
                           (ELEMS), (_n - _first_chunk) * sizeof(*(ELEMS)));  \
                }                                                             \
            }                                                                 \
                                                                              \
            (QUEUE).head = (_head + _n) & _mask;                              \
            (QUEUE).size -= _n;                                               \
        }                                                                     \
    } while (0)

#endif