#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Ring buffer. */
struct queue_info {
    size_t head;
    size_t tail;
    size_t size;
    size_t cap; /* Capacity must be a power of two! */
};

#define QUEUE_PUSH(ELEMS, INFO, SRC)                                          \
    do {                                                                      \
                                                                              \
    } while (0)

#define QUEUE_POP(ELEMS, INFO, DST)                                           \
    do {                                                                      \
                                                                              \
    } while (0)

#endif