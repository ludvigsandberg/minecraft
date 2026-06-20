#include "common/arr_s32.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/preproc.h"

s32 *arr_s32_at(arr_s32_t *arr, size_t i) {
    assert(arr);
    assert(i < arr->size);

    return arr->data + i;
}

size_t arr_s32_size(const arr_s32_t *arr) {
    assert(arr);

    return arr->size;
}

size_t arr_s32_cap(const arr_s32_t *arr) {
    assert(arr);

    return arr->cap;
}

void arr_s32_new(arr_s32_t *arr) {
    assert(arr);

    arr->data = NULL;
    arr->size = 0;
    arr->cap  = 0;
}

void arr_s32_new_n(arr_s32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_s32_new(arr);
        return;
    }

    arr->data = safe_malloc(n * sizeof(s32));
    arr->size = n;
    arr->cap  = n;
}

void arr_s32_new_n_zero(arr_s32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_s32_new(arr);
        return;
    }

    arr->data = safe_calloc(1, n * sizeof(s32));
    arr->size = n;
    arr->cap  = n;
}

void arr_s32_new_reserve(arr_s32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_s32_new(arr);
        return;
    }

    arr->data = safe_malloc(n * sizeof(s32));
    arr->size = 0;
    arr->cap  = n;
}

void arr_s32_new_reserve_zero(arr_s32_t *arr, size_t n) {
    assert(arr);

    if (n == 0) {
        arr_s32_new(arr);
        return;
    }

    arr->data = safe_calloc(1, n * sizeof(s32));
    arr->size = 0;
    arr->cap  = n;
}

void arr_s32_free(arr_s32_t *arr) {
    assert(arr);

    free(arr->data);

#ifndef NDEBUG
    arr->data = NULL;
    arr->size = 0;
    arr->cap  = 0;
#endif
}

void arr_s32_resize(arr_s32_t *arr, size_t n) {
    size_t new_cap;

    assert(arr);

    if (n == arr->size) {
        return;
    } else if (n <= arr->cap) {
        arr->size = n;
        return;
    }

    new_cap = arr->cap * 2;

    arr->data = safe_realloc(arr->data, new_cap * sizeof(s32));
    arr->size = n;
    arr->cap  = new_cap;
}

void arr_s32_insert_n_raw(arr_s32_t *arr, size_t i, size_t n) {
    size_t new_cap;

    assert(arr);
    assert(i <= arr->size);

    if ((arr->size + n) > arr->cap) {
        new_cap = MAX(arr->cap * 2, arr->size + n);

        arr->data = safe_realloc(arr->data, new_cap * sizeof(s32));
        arr->cap  = new_cap;
    }

    if (i != arr->size) {
        memmove(arr->data + i + n, arr->data + i,
                (arr->size - i) * sizeof(s32));
    }

    arr->size += n;
}

void arr_s32_insert_raw(arr_s32_t *arr, size_t i) {
    assert(arr);

    arr_s32_insert_n_raw(arr, i, 1);
}

void arr_s32_append_n_raw(arr_s32_t *arr, size_t n) {
    assert(arr);

    arr_s32_insert_n_raw(arr, arr->size, n);
}

void arr_s32_append_raw(arr_s32_t *arr) {
    assert(arr);

    arr_s32_append_n_raw(arr, 1);
}

void arr_s32_insert_n(arr_s32_t *dst, size_t i, size_t n, const s32 src[]) {
    assert(dst);
    assert(src);

    arr_s32_insert_n_raw(dst, i, n);
    memcpy(dst->data + i, src, n * sizeof(s32));
}

void arr_s32_insert(arr_s32_t *arr, size_t i, const s32 *val) {
    assert(arr);
    assert(val);

    arr_s32_insert_raw(arr, i);
    arr->data[i] = *val;
}

void arr_s32_append_n(arr_s32_t *dst, size_t n, const s32 src[]) {
    size_t i;

    assert(dst);
    assert(src);

    i = dst->size;

    arr_s32_insert_n_raw(dst, i, n);
    memcpy(dst->data + i, src, n * sizeof(s32));
}

void arr_s32_append(arr_s32_t *arr, const s32 *val) {
    size_t i;

    assert(arr);
    assert(val);

    i = arr->size;

    arr_s32_append_raw(arr);
    arr->data[i] = *val;
}

void arr_s32_remove_n(arr_s32_t *arr, size_t i, size_t n) {
    assert(arr);
    assert(i < arr->size);
    assert((i + n) <= arr->size);

    if ((i + n) < arr->size) {
        memmove(arr->data + i, arr->data + i + n,
                (arr->size - i - n) * sizeof(s32));
    }

    arr->size -= n;
}

void arr_s32_remove(arr_s32_t *arr, size_t i) {
    assert(arr);

    arr_s32_remove_n(arr, i, 1);
}

void arr_s32_remove_ptr(arr_s32_t *arr, const s32 *elem) {
    size_t i;

    assert(arr);
    assert(elem);

    i = elem - arr->data;

    arr_s32_remove(arr, i);
}