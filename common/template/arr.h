/*****************************************************************************
 * File:        arr_$NAME$.h
 * Author:      ludvigsandberg
 * Date:        2026-07-04
 * Description: DO NOT EDIT. Generated from common/template/arr.h.
 *****************************************************************************/

#ifndef $PROJECT$_ARR_$UPPER_NAME$_H
#define $PROJECT$_ARR_$UPPER_NAME$_H

#include <stddef.h>

$TYPE_INCLUDE$

typedef struct arr_$NAME$_s {
    $TYPE$ *data;
    size_t size;
    size_t cap;
} arr_$NAME$_t;

void arr_$NAME$_new(arr_$NAME$_t *dst);
void arr_$NAME$_new_n(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_new_n_zero(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_new_reserve(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_new_reserve_zero(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_free(arr_$NAME$_t *dst);
void arr_$NAME$_resize(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_insert_n_raw(arr_$NAME$_t *dst, size_t i, size_t n);
void arr_$NAME$_insert_raw(arr_$NAME$_t *dst, size_t i);
void arr_$NAME$_append_n_raw(arr_$NAME$_t *dst, size_t n);
void arr_$NAME$_append_raw(arr_$NAME$_t *dst);
void arr_$NAME$_insert_n(arr_$NAME$_t *dst, size_t i, size_t n, $TYPE$ *src);
void arr_$NAME$_insert(arr_$NAME$_t *dst, size_t i, $TYPE$ *src);
void arr_$NAME$_append_n(arr_$NAME$_t *dst, size_t n, $TYPE$ *src);
void arr_$NAME$_append(arr_$NAME$_t *dst, $TYPE$ *src);
void arr_$NAME$_remove_n(arr_$NAME$_t *dst, size_t i, size_t n);
void arr_$NAME$_remove(arr_$NAME$_t *dst, size_t i);

#endif
