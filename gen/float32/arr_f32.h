/* Generated from shared/inc/shared/arr_s32.h - DO NOT EDIT */

#ifndef SHARED_ARR_f32_H
#define SHARED_ARR_f32_H

#include <stddef.h>

#include "shared/types_f32.h"
#include "shared/mem.h"

typedef struct arr_f32_s {
    f32 *data;
    size_t size;
    size_t cap;
} arr_f32_t;

f32 *arr_f32_at(arr_f32_t *arr, size_t i);

size_t arr_f32_size(const arr_f32_t *arr);
size_t arr_f32_cap(const arr_f32_t *arr);

void arr_f32_new(arr_f32_t *arr);
void arr_f32_new_n(arr_f32_t *arr, size_t n);
void arr_f32_new_n_zero(arr_f32_t *arr, size_t n);
void arr_f32_new_reserve(arr_f32_t *arr, size_t n);
void arr_f32_new_reserve_zero(arr_f32_t *arr, size_t n);
void arr_f32_free(arr_f32_t *arr);

void arr_f32_resize(arr_f32_t *arr, size_t n);
void arr_f32_insert_n_raw(arr_f32_t *arr, size_t i, size_t n);
void arr_f32_insert_raw(arr_f32_t *arr, size_t i);
void arr_f32_append_n_raw(arr_f32_t *arr, size_t n);
void arr_f32_append_raw(arr_f32_t *arr);
void arr_f32_insert_n(arr_f32_t *dst, size_t i, size_t n, const f32 src[]);
void arr_f32_insert(arr_f32_t *arr, size_t i, const f32 *val);
void arr_f32_append_n(arr_f32_t *dst, size_t n, const f32 src[]);
void arr_f32_append(arr_f32_t *arr, const f32 *val);
void arr_f32_remove_n(arr_f32_t *arr, size_t i, size_t n);
void arr_f32_remove(arr_f32_t *arr, size_t i);
void arr_f32_remove_ptr(arr_f32_t *arr, const f32 *elem);

#endif
