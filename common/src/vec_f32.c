/*****************************************************************************
 * File:        vec_f32.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "common/vec_f32.h"

#include <math.h>
#include <assert.h>

void vec2_f32_add(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
}

void vec2_f32_sub(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
}

void vec2_f32_mul(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
}

void vec2_f32_div(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
}

f32 vec2_f32_len(const vec2_f32_t *vec) {
    assert(vec);

    return (f32)sqrt(
        (double)(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y));
}

void vec2_f32_norm(vec2_f32_t *dst, const vec2_f32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec2_f32_len(src);

    dst->pos.x = (f32)(src->pos.x / len);
    dst->pos.y = (f32)(src->pos.y / len);
}

void vec2_f32_dot(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y;
}

void vec3_f32_add(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
    dst->pos.z = a->pos.z + b->pos.z;
}

void vec3_f32_sub(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
    dst->pos.z = a->pos.z - b->pos.z;
}

void vec3_f32_mul(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
    dst->pos.z = a->pos.z * b->pos.z;
}

void vec3_f32_div(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
    dst->pos.z = a->pos.z / b->pos.z;
}

f32 vec3_f32_len(const vec3_f32_t *vec) {
    assert(vec);

    return (f32)sqrt((double)(vec->pos.x * vec->pos.x +
                              vec->pos.y * vec->pos.y +
                              vec->pos.z * vec->pos.z));
}

void vec3_f32_norm(vec3_f32_t *dst, const vec3_f32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec3_f32_len(src);

    dst->pos.x = (f32)(src->pos.x / len);
    dst->pos.y = (f32)(src->pos.y / len);
    dst->pos.z = (f32)(src->pos.z / len);
}

void vec3_f32_dot(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x =
        a->pos.x * b->pos.x + a->pos.y * b->pos.y + a->pos.z * b->pos.z;
}

void vec4_f32_add(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
    dst->pos.z = a->pos.z + b->pos.z;
    dst->pos.w = a->pos.w + b->pos.w;
}

void vec4_f32_sub(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
    dst->pos.z = a->pos.z - b->pos.z;
    dst->pos.w = a->pos.w - b->pos.w;
}

void vec4_f32_mul(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
    dst->pos.z = a->pos.z * b->pos.z;
    dst->pos.w = a->pos.w * b->pos.w;
}

void vec4_f32_div(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
    dst->pos.z = a->pos.z / b->pos.z;
    dst->pos.w = a->pos.w / b->pos.w;
}

f32 vec4_f32_len(const vec4_f32_t *vec) {
    assert(vec);

    return (f32)sqrt(
        (double)(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y +
                 vec->pos.z * vec->pos.z + vec->pos.w * vec->pos.w));
}

void vec4_f32_norm(vec4_f32_t *dst, const vec4_f32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec4_f32_len(src);

    dst->pos.x = (f32)(src->pos.x / len);
    dst->pos.y = (f32)(src->pos.y / len);
    dst->pos.z = (f32)(src->pos.z / len);
    dst->pos.w = (f32)(src->pos.w / len);
}

void vec4_f32_dot(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y +
                 a->pos.z * b->pos.z + a->pos.w * b->pos.w;
}
