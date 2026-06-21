/*****************************************************************************
 * File:        arr.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef COMMON_ARR_H
#define COMMON_ARR_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "common/mem.h"
#include "common/pp.h"

#define ARR_DECL(TYPE, NAME)                                                  \
    typedef struct arr_##NAME##_s {                                           \
        TYPE *data;                                                           \
        size_t size;                                                          \
        size_t cap;                                                           \
    } arr_##NAME##_t;

#define ARR_AT(ARR, I) ((ARR)->data + (I))
#define ARR_SIZE(ARR)  ((ARR)->size)
#define ARR_CAP(ARR)   ((ARR)->cap)

#define ARR_NEW(ARR)                                                          \
    do {                                                                      \
        (ARR)->data = NULL;                                                   \
        (ARR)->size = 0;                                                      \
        (ARR)->cap  = 0;                                                      \
    } while (0)

#define ARR_NEW_N(ARR, N)                                                     \
    do {                                                                      \
        size_t _n = (N);                                                      \
        if (_n == 0) {                                                        \
            ARR_NEW(ARR);                                                     \
        } else {                                                              \
            (ARR)->data = checked_malloc(_n * sizeof(*(ARR)->data));          \
            (ARR)->size = _n;                                                 \
            (ARR)->cap  = _n;                                                 \
        }                                                                     \
    } while (0)

#define ARR_NEW_N_ZERO(ARR, N)                                                \
    do {                                                                      \
        size_t _n = (N);                                                      \
        if (_n == 0) {                                                        \
            ARR_NEW(ARR);                                                     \
        } else {                                                              \
            (ARR)->data = checked_calloc(_n, sizeof(*(ARR)->data));           \
            (ARR)->size = _n;                                                 \
            (ARR)->cap  = _n;                                                 \
        }                                                                     \
    } while (0)

#define ARR_NEW_RESERVE(ARR, N)                                               \
    do {                                                                      \
        size_t _n = (N);                                                      \
        if (_n == 0) {                                                        \
            ARR_NEW(ARR);                                                     \
        } else {                                                              \
            (ARR)->data = checked_malloc(_n * sizeof(*(ARR)->data));          \
            (ARR)->size = 0;                                                  \
            (ARR)->cap  = _n;                                                 \
        }                                                                     \
    } while (0)

#define ARR_NEW_RESERVE_ZERO(ARR, N)                                          \
    do {                                                                      \
        size_t _n = (N);                                                      \
        if (_n == 0) {                                                        \
            ARR_NEW(ARR);                                                     \
        } else {                                                              \
            (ARR)->data = checked_calloc(_n, sizeof(*(ARR)->data));           \
            (ARR)->size = 0;                                                  \
            (ARR)->cap  = _n;                                                 \
        }                                                                     \
    } while (0)

#define ARR_FREE(ARR) free((ARR)->data);

#define ARR_RESIZE(ARR, N)                                                    \
    do {                                                                      \
        size_t _n = (N), _nc;                                                 \
        if (_n != (ARR)->size) {                                              \
            if (_n <= (ARR)->cap) {                                           \
                (ARR)->size = _n;                                             \
            } else {                                                          \
                _nc = (ARR)->cap * 2;                                         \
                (ARR)->data =                                                 \
                    checked_realloc((ARR)->data, _nc * sizeof(*(ARR)->data)); \
                (ARR)->size = _n;                                             \
                (ARR)->cap  = _nc;                                            \
            }                                                                 \
        }                                                                     \
    } while (0)

#define ARR_INSERT_N_RAW(ARR, I, N)                                           \
    do {                                                                      \
        size_t _nc;                                                           \
        if ((ARR)->size + (N) > (ARR)->cap) {                                 \
            _nc = MAX((ARR)->cap * 2, (ARR)->size + (N));                     \
            (ARR)->data =                                                     \
                checked_realloc((ARR)->data, _nc * sizeof(*(ARR)->data));     \
            (ARR)->cap = _nc;                                                 \
        }                                                                     \
        if ((I) != (ARR)->size) {                                             \
            memmove((ARR)->data + (I) + (N), (ARR)->data + (I),               \
                    ((ARR)->size - (I)) * sizeof(*(ARR)->data));              \
        }                                                                     \
        (ARR)->size += (N);                                                   \
    } while (0)

#define ARR_INSERT_RAW(ARR, I) ARR_INSERT_N_RAW((ARR), (I), 1);

#define ARR_APPEND_N_RAW(ARR, N) ARR_INSERT_N_RAW((ARR), (ARR)->size, (N));

#define ARR_APPEND_RAW(ARR) ARR_APPEND_N_RAW((ARR), 1);

#define ARR_INSERT_N(DST, I, N, SRC)                                          \
    do {                                                                      \
        ARR_INSERT_N_RAW((DST), (I), (N));                                    \
        memcpy((DST)->data + (I), (SRC), (N) * sizeof(*(DST)->data));         \
    } while (0)

#define ARR_INSERT(ARR, I, VAL)                                               \
    do {                                                                      \
        ARR_INSERT_RAW((ARR), (I));                                           \
        (ARR)->data[(I)] = *(VAL);                                            \
    } while (0)

#define ARR_APPEND_N(DST, N, SRC)                                             \
    do {                                                                      \
        size_t _i = (DST)->size;                                              \
        ARR_INSERT_N_RAW((DST), _i, (N));                                     \
        memcpy((DST)->data + _i, (SRC), (N) * sizeof(*(DST)->data));          \
    } while (0)

#define ARR_APPEND(ARR, VAL)                                                  \
    do {                                                                      \
        size_t _i = (ARR)->size;                                              \
        ARR_APPEND_RAW(ARR);                                                  \
        (ARR)->data[_i] = *(VAL);                                             \
    } while (0)

#define ARR_REMOVE_N(ARR, I, N)                                               \
    do {                                                                      \
        if ((I) + (N) < (ARR)->size) {                                        \
            memmove((ARR)->data + (I), (ARR)->data + (I) + (N),               \
                    ((ARR)->size - (I) - (N)) * sizeof(*(ARR)->data));        \
        }                                                                     \
        (ARR)->size -= (N);                                                   \
    } while (0)

#define ARR_REMOVE(ARR, I) ARR_REMOVE_N((ARR), (I), 1);

#define ARR_REMOVE_PTR(ARR, ELEM)                                             \
    ARR_REMOVE((ARR), (size_t)((ELEM) - (ARR)->data));

#endif
