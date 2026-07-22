#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARR_APPEND(ARR, COUNT, CAPACITY, SRC)                                 \
    ARR_APPEND_N(ARR, COUNT, CAPACITY, 1, SRC)

#define ARR_APPEND_N(ARR, COUNT, CAPACITY, N, SRC)                            \
    ARR_INSERT_N(ARR, COUNT, CAPACITY, COUNT, N, SRC)

#define ARR_INSERT(ARR, COUNT, CAPACITY, I, SRC)                              \
    ARR_INSERT_N(ARR, COUNT, CAPACITY, I, 1, SRC)

#define ARR_INSERT_N(ARR, COUNT, CAPACITY, I, N, SRC)                         \
    do {                                                                      \
        assert((I) <= (COUNT));                                               \
                                                                              \
        if ((COUNT) + (N) > (CAPACITY)) {                                     \
            (CAPACITY) = ((COUNT) + (N)) * 2;                                 \
            (ARR)      = realloc((ARR), (CAPACITY) * sizeof(*(ARR)));         \
        }                                                                     \
                                                                              \
        if ((I) != (COUNT)) {                                                 \
            memmove((ARR) + (I) + (N), (ARR) + (I),                           \
                    ((COUNT) - (I)) * sizeof(*(ARR)));                        \
        }                                                                     \
                                                                              \
        memcpy((ARR) + (I), (SRC), (N) * sizeof(*(ARR)));                     \
                                                                              \
        (COUNT) += (N);                                                       \
    } while (0)

#define ARR_REMOVE(ARR, COUNT, I) ARR_REMOVE_N(ARR, COUNT, I, 1)

#define ARR_REMOVE_N(ARR, COUNT, I, N)                                        \
    do {                                                                      \
        assert((I) < (COUNT));                                                \
        assert((I) + (N) <= (COUNT));                                         \
                                                                              \
        if ((I) + (N) < (COUNT)) {                                            \
            memmove((ARR) + (I), (ARR) + (I) + (N),                           \
                    ((COUNT) - (I) - (N)) * sizeof(*(ARR)));                  \
        }                                                                     \
                                                                              \
        (COUNT) -= (N);                                                       \
    } while (0)

#endif