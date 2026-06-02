/* This file was generated. Do not edit. */

#include "shared/arr_u32.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shared/preproc.h"

u32 *arr_u32_at(arr_u32_t *arr, size_t i) {
    assert(arr);
    assert(i < arr->size);

    return arr->data + i;
}

size_t arr_u32_size(const arr_u32_t *arr) {
    assert(arr);

    return arr->size;
}

size_t arr_u32_cap(const arr_u32_t *arr) {
    assert(arr);

    return arr->cap;
}

void arr_u32_new(arr_u32_t *arr) {
    assert(arr);

    arr->data = NULL;
    arr->size = 0;
    arr->cap  = 0;
}

void arr_u32_new_n(arr_u32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_u32_new(arr);
        return;
    }

    arr->data = safe_malloc(n * sizeof(u32));
    arr->size = n;
    arr->cap  = n;
}

void arr_u32_new_n_zero(arr_u32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_u32_new(arr);
        return;
    }

    arr->data = safe_calloc(1, n * sizeof(u32));
    arr->size = n;
    arr->cap  = n;
}

void arr_u32_new_reserve(arr_u32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_u32_new(arr);
        return;
    }

    arr->data = safe_malloc(n * sizeof(u32));
    arr->size = 0;
    arr->cap  = n;
}

void arr_u32_new_reserve_zero(arr_u32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_u32_new(arr);
        return;
    }

    arr->data = safe_calloc(1, n * sizeof(u32));
    arr->size = 0;
    arr->cap  = n;
}

void arr_u32_free(arr_u32_t *arr) {
    assert(arr);

    free(arr->data);

#ifndef NDEBUG
    arr->data = NULL;
    arr->size = 0;
    arr->cap  = 0;
#endif
}

void arr_u32_resize(arr_u32_t *arr, size_t n) {
    size_t new_cap;

    assert(arr);

    if (n == arr->size) {
        return;
    } else if (n <= arr->cap) {
        arr->size = n;
        return;
    }

    new_cap = arr->cap * 2;

    arr->data = safe_realloc(arr->data, new_cap * sizeof(u32));
    arr->size = n;
    arr->cap  = new_cap;
}

void arr_u32_insert_n_raw(arr_u32_t *arr, size_t i, size_t n) {
    size_t new_cap;

    assert(arr);
    assert(i <= arr->size);

    if ((arr->size + n) > arr->cap) {
        new_cap = MAX(arr->cap * 2, arr->size + n);

        arr->data = safe_realloc(arr->data, new_cap * sizeof(u32));
        arr->cap  = new_cap;
    }

    if (i != arr->size) {
        memmove(arr->data + i + n, arr->data + i,
                (arr->size - i) * sizeof(u32));
    }

    arr->size += n;
}

void arr_u32_insert_raw(arr_u32_t *arr, size_t i) {
    assert(arr);

    arr_u32_insert_n_raw(arr, i, 1);
}

void arr_u32_append_n_raw(arr_u32_t *arr, size_t n) {
    assert(arr);

    arr_u32_insert_n_raw(arr, arr->size, n);
}

void arr_u32_append_raw(arr_u32_t *arr) {
    assert(arr);

    arr_u32_append_n_raw(arr, 1);
}

void arr_u32_insert_n(arr_u32_t *dst, size_t i, size_t n, const u32 src[]) {
    assert(dst);
    assert(src);

    arr_u32_insert_n_raw(dst, i, n);
    memcpy(dst->data + i, src, n * sizeof(u32));
}

void arr_u32_insert(arr_u32_t *arr, size_t i, const u32 *val) {
    assert(arr);
    assert(val);

    arr_u32_insert_raw(arr, i);
    arr->data[i] = *val;
}

void arr_u32_append_n(arr_u32_t *dst, size_t n, const u32 src[]) {
    size_t i;

    assert(dst);
    assert(src);

    i = dst->size;

    arr_u32_insert_n_raw(dst, i, n);
    memcpy(dst->data + i, src, n * sizeof(u32));
}

void arr_u32_append(arr_u32_t *arr, const u32 *val) {
    size_t i;

    assert(arr);
    assert(val);

    i = arr->size;

    arr_u32_append_raw(arr);
    arr->data[i] = *val;
}

void arr_u32_remove_n(arr_u32_t *arr, size_t i, size_t n) {
    assert(arr);
    assert(i < arr->size);
    assert((i + n) <= arr->size);

    if ((i + n) < arr->size) {
        memmove(arr->data + i, arr->data + i + n,
                (arr->size - i - n) * sizeof(u32));
    }

    arr->size -= n;
}

void arr_u32_remove(arr_u32_t *arr, size_t i) {
    assert(arr);

    arr_u32_remove_n(arr, i, 1);
}

void arr_u32_remove_ptr(arr_u32_t *arr, const u32 *elem) {
    size_t i;

    assert(arr);
    assert(elem);

    i = elem - arr->data;

    arr_u32_remove(arr, i);
}
