/*****************************************************************************
 * File:        arr_$NAME$.c
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/arr.c.
 *****************************************************************************/

$SELF_INCLUDE$

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/mem.h"

void arr_$NAME$_new(arr_$NAME$_t *dst) {
    assert(dst);

    dst->data = NULL;
    dst->size = 0;
    dst->cap  = 0;
}

void arr_$NAME$_new_n(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    dst->data = checked_malloc(n * sizeof($TYPE$));
    dst->size = n;
    dst->cap  = n;
}

void arr_$NAME$_new_n_zero(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    dst->data = checked_calloc(n, sizeof($TYPE$));
    dst->size = n;
    dst->cap  = n;
}

void arr_$NAME$_new_reserve(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    dst->data = checked_malloc(n * sizeof($TYPE$));
    dst->size = 0;
    dst->cap  = n;
}

void arr_$NAME$_new_reserve_zero(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    dst->data = checked_calloc(n, sizeof($TYPE$));
    dst->size = 0;
    dst->cap  = n;
}

void arr_$NAME$_free(arr_$NAME$_t *dst) {
    assert(dst);

    free(dst->data);
    dst->data = NULL;
    dst->size = 0;
    dst->cap  = 0;
}

void arr_$NAME$_resize(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    dst->data = checked_realloc(dst->data, n * sizeof($TYPE$));
    dst->size = n;
    dst->cap  = n;
}

void arr_$NAME$_insert_n_raw(arr_$NAME$_t *dst, size_t i, size_t n) {
    assert(dst);
    assert(i <= dst->size);

    if (dst->size + n > dst->cap) {
        dst->cap = (dst->size + n) * 2;

        dst->data = checked_realloc(dst->data, dst->cap * sizeof($TYPE$));
    }

    if (i != dst->size) {
        memmove(dst->data + i + n, dst->data + i,
                (dst->size - i) * sizeof($TYPE$));
    }

    dst->size += n;
}

void arr_$NAME$_insert_raw(arr_$NAME$_t *dst, size_t i) {
    assert(dst);

    arr_$NAME$_insert_n_raw(dst, i, 1);
}

void arr_$NAME$_append_n_raw(arr_$NAME$_t *dst, size_t n) {
    assert(dst);

    arr_$NAME$_insert_n_raw(dst, dst->size, n);
}

void arr_$NAME$_append_raw(arr_$NAME$_t *dst) {
    assert(dst);

    arr_$NAME$_append_n_raw(dst, 1);
}

void arr_$NAME$_insert_n(arr_$NAME$_t *dst, size_t i, size_t n, $TYPE$ *src) {
    assert(dst);
    assert(src);

    arr_$NAME$_insert_n_raw(dst, i, n);
    memcpy(dst->data + i, src, n * sizeof($TYPE$));
}

void arr_$NAME$_insert(arr_$NAME$_t *dst, size_t i, $TYPE$ *src) {
    assert(dst);
    assert(src);

    arr_$NAME$_insert_n(dst, i, 1, src);
}

void arr_$NAME$_append_n(arr_$NAME$_t *dst, size_t n, $TYPE$ *src) {
    assert(dst);
    assert(src);

    arr_$NAME$_insert_n(dst, dst->size, n, src);
}

void arr_$NAME$_append(arr_$NAME$_t *dst, $TYPE$ *src) {
    assert(dst);
    assert(src);

    arr_$NAME$_append_n(dst, 1, src);
}

void arr_$NAME$_remove_n(arr_$NAME$_t *dst, size_t i, size_t n) {
    assert(dst);
    assert(i < dst->size);
    assert(i + n <= dst->size);

    if (i + n < dst->size) {
        memmove(dst->data + i, dst->data + i + n,
                (dst->size - i - n) * sizeof($TYPE$));
    }

    dst->size -= n;
}

void arr_$NAME$_remove(arr_$NAME$_t *dst, size_t i) {
    assert(dst);

    arr_$NAME$_remove_n(dst, i, 1);
}
