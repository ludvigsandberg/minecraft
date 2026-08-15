#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Dynamic array. */
struct array {
    size_t size;
    size_t capacity;
};

#define ARRAY_INSERT(ARRAY, ELEMS, I, ITEM)                                   \
    do {                                                                      \
        size_t _i = (I);                                                      \
        assert(_i <= (ARRAY).size);                                           \
                                                                              \
        if ((ARRAY).size + 1 > (ARRAY).capacity) {                            \
            size_t _new_cap =                                                 \
                ((ARRAY).capacity == 0) ? 16 : ((ARRAY).capacity * 2);        \
            (ELEMS) = realloc((ELEMS), _new_cap * sizeof(*(ELEMS)));          \
            if (!(ELEMS)) {                                                   \
                printf("%s:%d Out of memory!\n", __FILE__, __LINE__);         \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
            (ARRAY).capacity = _new_cap;                                      \
        }                                                                     \
                                                                              \
        if (_i != (ARRAY).size) {                                             \
            memmove((ELEMS) + _i + 1, (ELEMS) + _i,                           \
                    ((ARRAY).size - _i) * sizeof(*(ELEMS)));                  \
        }                                                                     \
                                                                              \
        (ELEMS)[_i] = (ITEM);                                                 \
        (ARRAY).size++;                                                       \
    } while (0)

#define ARRAY_INSERT_N(ARRAY, ELEMS, I, N, SRC)                               \
    do {                                                                      \
        size_t _i = (I);                                                      \
        size_t _n = (N);                                                      \
        assert(_i <= (ARRAY).size);                                           \
                                                                              \
        if ((ARRAY).size + _n > (ARRAY).capacity) {                           \
            size_t _new_cap =                                                 \
                ((ARRAY).capacity == 0) ? 16 : (ARRAY).capacity;              \
            while (_new_cap < (ARRAY).size + _n) {                            \
                _new_cap *= 2;                                                \
            }                                                                 \
            (ELEMS) = realloc((ELEMS), _new_cap * sizeof(*(ELEMS)));          \
            if (!(ELEMS)) {                                                   \
                printf("%s:%d Out of memory!\n", __FILE__, __LINE__);         \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
            (ARRAY).capacity = _new_cap;                                      \
        }                                                                     \
                                                                              \
        if (_i != (ARRAY).size) {                                             \
            memmove((ELEMS) + _i + _n, (ELEMS) + _i,                          \
                    ((ARRAY).size - _i) * sizeof(*(ELEMS)));                  \
        }                                                                     \
                                                                              \
        memcpy((ELEMS) + _i, (SRC), _n * sizeof(*(ELEMS)));                   \
        (ARRAY).size += _n;                                                   \
    } while (0)

#define ARRAY_APPEND(ARRAY, ELEMS, ITEM)                                      \
    do {                                                                      \
        if ((ARRAY).size + 1 > (ARRAY).capacity) {                            \
            size_t _new_cap =                                                 \
                ((ARRAY).capacity == 0) ? 16 : ((ARRAY).capacity * 2);        \
            (ELEMS) = realloc((ELEMS), _new_cap * sizeof(*(ELEMS)));          \
            if (!(ELEMS)) {                                                   \
                printf("%s:%d Out of memory!\n", __FILE__, __LINE__);         \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
            (ARRAY).capacity = _new_cap;                                      \
        }                                                                     \
        (ELEMS)[(ARRAY).size++] = (ITEM);                                     \
    } while (0)

#define ARRAY_APPEND_N(ARRAY, ELEMS, N, SRC)                                  \
    ARRAY_INSERT_N(ARRAY, ELEMS, (ARRAY).size, N, SRC)

#define ARRAY_REMOVE(ARRAY, ELEMS, I)                                         \
    do {                                                                      \
        size_t _i = (I);                                                      \
        assert(_i < (ARRAY).size);                                            \
                                                                              \
        if (_i + 1 < (ARRAY).size) {                                          \
            memmove((ELEMS) + _i, (ELEMS) + _i + 1,                           \
                    ((ARRAY).size - _i - 1) * sizeof(*(ELEMS)));              \
        }                                                                     \
                                                                              \
        (ARRAY).size--;                                                       \
    } while (0)

#define ARRAY_REMOVE_N(ARRAY, ELEMS, I, N)                                    \
    do {                                                                      \
        size_t _i = (I);                                                      \
        size_t _n = (N);                                                      \
        assert(_i < (ARRAY).size);                                            \
        assert(_i + _n <= (ARRAY).size);                                      \
                                                                              \
        if (_i + _n < (ARRAY).size) {                                         \
            memmove((ELEMS) + _i, (ELEMS) + _i + _n,                          \
                    ((ARRAY).size - _i - _n) * sizeof(*(ELEMS)));             \
        }                                                                     \
                                                                              \
        (ARRAY).size -= _n;                                                   \
    } while (0)

#define ARRAY_POP(ARRAY, ELEMS, DST)                                          \
    do {                                                                      \
        assert((ARRAY).size > 0);                                             \
        (DST) = (ELEMS)[--(ARRAY).size];                                      \
    } while (0)

#endif