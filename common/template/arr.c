/*****************************************************************************
 * File:        arr_$NAME$.c
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/arr.c.
 *****************************************************************************/

#include "$SELF_INCLUDE$"

#include <stdlib.h>
#include <string.h>

static void arr_$NAME$_grow(arr_$NAME$_t *a, size_t needed) {
    size_t nc;
    if (needed <= a->cap)
        return;
    nc = a->cap ? a->cap * 2 : 4;
    if (nc < needed)
        nc = needed;
    a->data = realloc(a->data, nc * sizeof(*a->data));
    if (!a->data)
        abort();
    a->cap = nc;
}

void arr_$NAME$_new(arr_$NAME$_t *a) {
    a->data = NULL;
    a->size = 0;
    a->cap  = 0;
}

void arr_$NAME$_new_n(arr_$NAME$_t *a, size_t n) {
    if (n == 0) {
        arr_$NAME$_new(a);
        return;
    }
    a->data = malloc(n * sizeof(*a->data));
    if (!a->data)
        abort();
    a->size = n;
    a->cap  = n;
}

void arr_$NAME$_free(arr_$NAME$_t *a) {
    free(a->data);
}

$TYPE$ *arr_$NAME$_at(arr_$NAME$_t *a, size_t i) {
    return a->data + i;
}

size_t arr_$NAME$_size(const arr_$NAME$_t *a) {
    return a->size;
}

size_t arr_$NAME$_cap(const arr_$NAME$_t *a) {
    return a->cap;
}

void arr_$NAME$_resize(arr_$NAME$_t *a, size_t n) {
    if (n == a->size)
        return;
    arr_$NAME$_grow(a, n);
    a->size = n;
}

void arr_$NAME$_append(arr_$NAME$_t *a, $TYPE$ val) {
    arr_$NAME$_grow(a, a->size + 1);
    a->data[a->size++] = val;
}

void arr_$NAME$_append_n(arr_$NAME$_t *a, size_t n, const $TYPE$ *src) {
    arr_$NAME$_grow(a, a->size + n);
    memcpy(a->data + a->size, src, n * sizeof(*a->data));
    a->size += n;
}

void arr_$NAME$_insert(arr_$NAME$_t *a, size_t i, $TYPE$ val) {
    arr_$NAME$_grow(a, a->size + 1);
    if (i != a->size)
        memmove(a->data + i + 1, a->data + i,
                (a->size - i) * sizeof(*a->data));
    a->data[i] = val;
    a->size++;
}

void arr_$NAME$_insert_n(arr_$NAME$_t *a, size_t i, size_t n,
                         const $TYPE$ *src) {
    arr_$NAME$_grow(a, a->size + n);
    if (i != a->size)
        memmove(a->data + i + n, a->data + i,
                (a->size - i) * sizeof(*a->data));
    memcpy(a->data + i, src, n * sizeof(*a->data));
    a->size += n;
}

void arr_$NAME$_remove_n(arr_$NAME$_t *a, size_t i, size_t n) {
    if (i + n < a->size)
        memmove(a->data + i, a->data + i + n,
                (a->size - i - n) * sizeof(*a->data));
    a->size -= n;
}

void arr_$NAME$_remove(arr_$NAME$_t *a, size_t i) {
    arr_$NAME$_remove_n(a, i, 1);
}
