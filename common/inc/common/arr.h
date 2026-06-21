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
#include <assert.h>

#include "common/mem.h"
#include "common/pp.h"

#define ARR_DECL(TYPE, NAME)                                                  \
    typedef struct arr_##NAME##_s {                                           \
        TYPE *data;                                                           \
        size_t size;                                                          \
        size_t cap;                                                           \
    } arr_##NAME##_t;                                                         \
                                                                              \
    TYPE *arr_##NAME##_at(arr_##NAME##_t *arr, size_t i);                     \
                                                                              \
    size_t arr_##NAME##_size(const arr_##NAME##_t *arr);                      \
    size_t arr_##NAME##_cap(const arr_##NAME##_t *arr);                       \
                                                                              \
    void arr_##NAME##_new(arr_##NAME##_t *arr);                               \
    void arr_##NAME##_new_n(arr_##NAME##_t *arr, size_t n);                   \
    void arr_##NAME##_new_n_zero(arr_##NAME##_t *arr, size_t n);              \
    void arr_##NAME##_new_reserve(arr_##NAME##_t *arr, size_t n);             \
    void arr_##NAME##_new_reserve_zero(arr_##NAME##_t *arr, size_t n);        \
    void arr_##NAME##_free(arr_##NAME##_t *arr);                              \
                                                                              \
    void arr_##NAME##_resize(arr_##NAME##_t *arr, size_t n);                  \
    void arr_##NAME##_insert_n_raw(arr_##NAME##_t *arr, size_t i, size_t n);  \
    void arr_##NAME##_insert_raw(arr_##NAME##_t *arr, size_t i);              \
    void arr_##NAME##_append_n_raw(arr_##NAME##_t *arr, size_t n);            \
    void arr_##NAME##_append_raw(arr_##NAME##_t *arr);                        \
    void arr_##NAME##_insert_n(arr_##NAME##_t *dst, size_t i, size_t n,       \
                               const TYPE src[]);                             \
    void arr_##NAME##_insert(arr_##NAME##_t *arr, size_t i, const TYPE *val); \
    void arr_##NAME##_append_n(arr_##NAME##_t *dst, size_t n,                 \
                               const TYPE src[]);                             \
    void arr_##NAME##_append(arr_##NAME##_t *arr, const TYPE *val);           \
    void arr_##NAME##_remove_n(arr_##NAME##_t *arr, size_t i, size_t n);      \
    void arr_##NAME##_remove(arr_##NAME##_t *arr, size_t i);                  \
    void arr_##NAME##_remove_ptr(arr_##NAME##_t *arr, const TYPE *elem);

#define ARR_DEF(TYPE, NAME)                                                   \
    TYPE *arr_##NAME##_at(arr_##NAME##_t *arr, size_t i) {                    \
        assert(arr);                                                          \
        assert(i < arr->size);                                                \
        return arr->data + i;                                                 \
    }                                                                         \
                                                                              \
    size_t arr_##NAME##_size(const arr_##NAME##_t *arr) {                     \
        assert(arr);                                                          \
        return arr->size;                                                     \
    }                                                                         \
                                                                              \
    size_t arr_##NAME##_cap(const arr_##NAME##_t *arr) {                      \
        assert(arr);                                                          \
        return arr->cap;                                                      \
    }                                                                         \
                                                                              \
    void arr_##NAME##_new(arr_##NAME##_t *arr) {                              \
        assert(arr);                                                          \
        arr->data = NULL;                                                     \
        arr->size = 0;                                                        \
        arr->cap  = 0;                                                        \
    }                                                                         \
                                                                              \
    void arr_##NAME##_new_n(arr_##NAME##_t *arr, size_t n) {                  \
        assert(arr);                                                          \
        if (n == 0) {                                                         \
            arr_##NAME##_new(arr);                                            \
            return;                                                           \
        }                                                                     \
        arr->data = checked_malloc(n * sizeof(TYPE));                         \
        arr->size = n;                                                        \
        arr->cap  = n;                                                        \
    }                                                                         \
                                                                              \
    void arr_##NAME##_new_n_zero(arr_##NAME##_t *arr, size_t n) {             \
        assert(arr);                                                          \
        if (n == 0) {                                                         \
            arr_##NAME##_new(arr);                                            \
            return;                                                           \
        }                                                                     \
        arr->data = checked_calloc(n, sizeof(TYPE));                          \
        arr->size = n;                                                        \
        arr->cap  = n;                                                        \
    }                                                                         \
                                                                              \
    void arr_##NAME##_new_reserve(arr_##NAME##_t *arr, size_t n) {            \
        assert(arr);                                                          \
        if (n == 0) {                                                         \
            arr_##NAME##_new(arr);                                            \
            return;                                                           \
        }                                                                     \
        arr->data = checked_malloc(n * sizeof(TYPE));                         \
        arr->size = 0;                                                        \
        arr->cap  = n;                                                        \
    }                                                                         \
                                                                              \
    void arr_##NAME##_new_reserve_zero(arr_##NAME##_t *arr, size_t n) {       \
        assert(arr);                                                          \
        if (n == 0) {                                                         \
            arr_##NAME##_new(arr);                                            \
            return;                                                           \
        }                                                                     \
        arr->data = checked_calloc(n, sizeof(TYPE));                          \
        arr->size = 0;                                                        \
        arr->cap  = n;                                                        \
    }                                                                         \
                                                                              \
    void arr_##NAME##_free(arr_##NAME##_t *arr) {                             \
        assert(arr);                                                          \
        free(arr->data);                                                      \
    }                                                                         \
                                                                              \
    void arr_##NAME##_resize(arr_##NAME##_t *arr, size_t n) {                 \
        size_t new_cap;                                                       \
        assert(arr);                                                          \
        if (n == arr->size) {                                                 \
            return;                                                           \
        } else if (n <= arr->cap) {                                           \
            arr->size = n;                                                    \
            return;                                                           \
        }                                                                     \
        new_cap   = arr->cap * 2;                                             \
        arr->data = checked_realloc(arr->data, new_cap * sizeof(TYPE));       \
        arr->size = n;                                                        \
        arr->cap  = new_cap;                                                  \
    }                                                                         \
                                                                              \
    void arr_##NAME##_insert_n_raw(arr_##NAME##_t *arr, size_t i, size_t n) { \
        size_t new_cap;                                                       \
        assert(arr);                                                          \
        assert(i <= arr->size);                                               \
        if ((arr->size + n) > arr->cap) {                                     \
            new_cap   = MAX(arr->cap * 2, arr->size + n);                     \
            arr->data = checked_realloc(arr->data, new_cap * sizeof(TYPE));   \
            arr->cap  = new_cap;                                              \
        }                                                                     \
        if (i != arr->size) {                                                 \
            memmove(arr->data + i + n, arr->data + i,                         \
                    (arr->size - i) * sizeof(TYPE));                          \
        }                                                                     \
        arr->size += n;                                                       \
    }                                                                         \
                                                                              \
    void arr_##NAME##_insert_raw(arr_##NAME##_t *arr, size_t i) {             \
        assert(arr);                                                          \
        arr_##NAME##_insert_n_raw(arr, i, 1);                                 \
    }                                                                         \
                                                                              \
    void arr_##NAME##_append_n_raw(arr_##NAME##_t *arr, size_t n) {           \
        assert(arr);                                                          \
        arr_##NAME##_insert_n_raw(arr, arr->size, n);                         \
    }                                                                         \
                                                                              \
    void arr_##NAME##_append_raw(arr_##NAME##_t *arr) {                       \
        assert(arr);                                                          \
        arr_##NAME##_append_n_raw(arr, 1);                                    \
    }                                                                         \
                                                                              \
    void arr_##NAME##_insert_n(arr_##NAME##_t *dst, size_t i, size_t n,       \
                               const TYPE src[]) {                            \
        assert(dst);                                                          \
        assert(src);                                                          \
        arr_##NAME##_insert_n_raw(dst, i, n);                                 \
        memcpy(dst->data + i, src, n * sizeof(TYPE));                         \
    }                                                                         \
                                                                              \
    void arr_##NAME##_insert(arr_##NAME##_t *arr, size_t i,                   \
                             const TYPE *val) {                               \
        assert(arr);                                                          \
        assert(val);                                                          \
        arr_##NAME##_insert_raw(arr, i);                                      \
        arr->data[i] = *val;                                                  \
    }                                                                         \
                                                                              \
    void arr_##NAME##_append_n(arr_##NAME##_t *dst, size_t n,                 \
                               const TYPE src[]) {                            \
        size_t i;                                                             \
        assert(dst);                                                          \
        assert(src);                                                          \
        i = dst->size;                                                        \
        arr_##NAME##_insert_n_raw(dst, i, n);                                 \
        memcpy(dst->data + i, src, n * sizeof(TYPE));                         \
    }                                                                         \
                                                                              \
    void arr_##NAME##_append(arr_##NAME##_t *arr, const TYPE *val) {          \
        size_t i;                                                             \
        assert(arr);                                                          \
        assert(val);                                                          \
        i = arr->size;                                                        \
        arr_##NAME##_append_raw(arr);                                         \
        arr->data[i] = *val;                                                  \
    }                                                                         \
                                                                              \
    void arr_##NAME##_remove_n(arr_##NAME##_t *arr, size_t i, size_t n) {     \
        assert(arr);                                                          \
        assert(i < arr->size);                                                \
        assert((i + n) <= arr->size);                                         \
        if ((i + n) < arr->size) {                                            \
            memmove(arr->data + i, arr->data + i + n,                         \
                    (arr->size - i - n) * sizeof(TYPE));                      \
        }                                                                     \
        arr->size -= n;                                                       \
    }                                                                         \
                                                                              \
    void arr_##NAME##_remove(arr_##NAME##_t *arr, size_t i) {                 \
        assert(arr);                                                          \
        arr_##NAME##_remove_n(arr, i, 1);                                     \
    }                                                                         \
                                                                              \
    void arr_##NAME##_remove_ptr(arr_##NAME##_t *arr, const TYPE *elem) {     \
        size_t i;                                                             \
        assert(arr);                                                          \
        assert(elem);                                                         \
        i = (TYPE *)elem - arr->data;                                         \
        arr_##NAME##_remove(arr, i);                                          \
    }

#endif
