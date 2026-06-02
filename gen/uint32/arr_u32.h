/* Generated from shared/inc/shared/arr_s32.h - DO NOT EDIT */

#ifndef SHARED_ARR_u32_H
#define SHARED_ARR_u32_H

#include <stddef.h>

#include "shared/types_u32.h"
#include "shared/mem.h"

typedef struct arr_u32_s {
    u32 *data;
    size_t size;
    size_t cap;
} arr_u32_t;

u32 *arr_u32_at(arr_u32_t *arr, size_t i);

size_t arr_u32_size(const arr_u32_t *arr);
size_t arr_u32_cap(const arr_u32_t *arr);

void arr_u32_new(arr_u32_t *arr);
void arr_u32_new_n(arr_u32_t *arr, size_t n);
void arr_u32_new_n_zero(arr_u32_t *arr, size_t n);
void arr_u32_new_reserve(arr_u32_t *arr, size_t n);
void arr_u32_new_reserve_zero(arr_u32_t *arr, size_t n);
void arr_u32_free(arr_u32_t *arr);

void arr_u32_resize(arr_u32_t *arr, size_t n);
void arr_u32_insert_n_raw(arr_u32_t *arr, size_t i, size_t n);
void arr_u32_insert_raw(arr_u32_t *arr, size_t i);
void arr_u32_append_n_raw(arr_u32_t *arr, size_t n);
void arr_u32_append_raw(arr_u32_t *arr);
void arr_u32_insert_n(arr_u32_t *dst, size_t i, size_t n, const u32 src[]);
void arr_u32_insert(arr_u32_t *arr, size_t i, const u32 *val);
void arr_u32_append_n(arr_u32_t *dst, size_t n, const u32 src[]);
void arr_u32_append(arr_u32_t *arr, const u32 *val);
void arr_u32_remove_n(arr_u32_t *arr, size_t i, size_t n);
void arr_u32_remove(arr_u32_t *arr, size_t i);
void arr_u32_remove_ptr(arr_u32_t *arr, const u32 *elem);

#endif
