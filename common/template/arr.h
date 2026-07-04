/*****************************************************************************
 * File:        arr_$NAME$.h
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/arr.h.
 *****************************************************************************/

#ifndef $PROJECT$_ARR_$UPPER_NAME$_H
#define $PROJECT$_ARR_$UPPER_NAME$_H

#include <stddef.h>

#include "$TYPE_INCLUDE$"

#define ARR_AT(A, I) (A).data[I]

typedef struct arr_$NAME$_s {
    $TYPE$ *data;
    size_t size;
    size_t cap;
} arr_$NAME$_t;

void arr_$NAME$_new(arr_$NAME$_t *a);
void arr_$NAME$_new_n(arr_$NAME$_t *a, size_t n);
void arr_$NAME$_free(arr_$NAME$_t *a);

void arr_$NAME$_resize(arr_$NAME$_t *a, size_t n);
void arr_$NAME$_append(arr_$NAME$_t *a, $TYPE$ val);
void arr_$NAME$_append_n(arr_$NAME$_t *a, size_t n, const $TYPE$ *src);
void arr_$NAME$_insert(arr_$NAME$_t *a, size_t i, $TYPE$ val);
void arr_$NAME$_insert_n(arr_$NAME$_t *a, size_t i, size_t n,
                         const $TYPE$ *src);
void arr_$NAME$_remove(arr_$NAME$_t *a, size_t i);
void arr_$NAME$_remove_n(arr_$NAME$_t *a, size_t i, size_t n);

#endif
