#ifndef SHARED_ARR_S32_H
#define SHARED_ARR_S32_H

#include <stddef.h>

#include "shared/types.h"
#include "shared/mem.h"

typedef struct arr_s32_s {
    s32 *data;
    size_t size;
    size_t cap;
} arr_s32_t;

s32 *arr_s32_at(arr_s32_t *arr, size_t i);

size_t arr_s32_size(const arr_s32_t *arr);
size_t arr_s32_cap(const arr_s32_t *arr);

void arr_s32_new(arr_s32_t *arr);
void arr_s32_new_n(arr_s32_t *arr, size_t n);
void arr_s32_new_n_zero(arr_s32_t *arr, size_t n);
void arr_s32_new_reserve(arr_s32_t *arr, size_t n);
void arr_s32_new_reserve_zero(arr_s32_t *arr, size_t n);
void arr_s32_free(arr_s32_t *arr);

void arr_s32_resize(arr_s32_t *arr, size_t n);
void arr_s32_insert_n_raw(arr_s32_t *arr, size_t i, size_t n);
void arr_s32_insert_raw(arr_s32_t *arr, size_t i);
void arr_s32_append_n_raw(arr_s32_t *arr, size_t n);
void arr_s32_append_raw(arr_s32_t *arr);
void arr_s32_insert_n(arr_s32_t *dst, size_t i, size_t n, const s32 src[]);
void arr_s32_insert(arr_s32_t *arr, size_t i, const s32 *val);
void arr_s32_append_n(arr_s32_t *dst, size_t n, const s32 src[]);
void arr_s32_append(arr_s32_t *arr, const s32 *val);
void arr_s32_remove_n(arr_s32_t *arr, size_t i, size_t n);
void arr_s32_remove(arr_s32_t *arr, size_t i);
void arr_s32_remove_ptr(arr_s32_t *arr, const s32 *elem);

#endif
