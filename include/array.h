#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct array_info {
    size_t size;
    size_t cap;
};

#define ARRAY_APPEND(ELEMS, INFO, SRC) ARRAY_APPEND_N(ELEMS, INFO, 1, SRC)

#define ARRAY_APPEND_N(ELEMS, INFO, N, SRC) ARRAY_INSERT_N(ELEMS, INFO, N, SRC)

#define ARRAY_INSERT(ELEMS, INFO, I, SRC)                                     \
    ARRAY_INSERT_N(ELEMS, INFO, I, 1, SRC)

#define ARRAY_INSERT_N(ELEMS, INFO, I, N, SRC)                                \
    do {                                                                      \
        assert((I) <= (INFO).size);                                           \
                                                                              \
        if ((INFO).size + (N) > (INFO).cap) {                                 \
            (INFO).cap = ((INFO).size + (N)) * 2;                             \
            (ELEMS)    = realloc((ELEMS), (INFO).cap * sizeof(*(ELEMS)));     \
            if (!(ELEMS)) {                                                   \
                printf("%s:%d Out of memory!\r\n", __FILE__, __LINE__);       \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
        }                                                                     \
                                                                              \
        if ((I) != (INFO).size) {                                             \
            memmove((ELEMS) + (I) + (N), (ELEMS) + (I),                       \
                    ((INFO).size - (I)) * sizeof(*(ELEMS)));                  \
        }                                                                     \
                                                                              \
        memcpy((ELEMS) + (I), (SRC), (N) * sizeof(*(ELEMS)));                 \
                                                                              \
        (INFO).size += (N);                                                   \
    } while (0)

#define ARRAY_REMOVE(ELEMS, INFO, I) ARRAY_REMOVE_N(ELEMS, INFO, I, 1)

#define ARRAY_REMOVE_N(ELEMS, INFO, I, N)                                     \
    do {                                                                      \
        assert((I) < (INFO).size);                                            \
        assert((I) + (N) <= (INFO).size);                                     \
                                                                              \
        if ((I) + (N) < (INFO).size) {                                        \
            memmove((ELEMS) + (I), (ELEMS) + (I) + (N),                       \
                    ((INFO).size - (I) - (N)) * sizeof(*(ELEMS)));            \
        }                                                                     \
                                                                              \
        (INFO).size -= (N);                                                   \
    } while (0)

#endif