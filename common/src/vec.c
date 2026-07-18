/*****************************************************************************
 * File:        vec.c
 * Author:      ludvigsandberg
 * Date:        2026-07-13
 * Description: -
 *****************************************************************************/

#include "common/vec.h"

#include <math.h>
#include <assert.h>

void vec2_add(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X + b->VEC_X;
    dst->VEC_Y = a->VEC_Y + b->VEC_Y;
}

void vec2_sub(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X - b->VEC_X;
    dst->VEC_Y = a->VEC_Y - b->VEC_Y;
}

void vec2_mul(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X * b->VEC_X;
    dst->VEC_Y = a->VEC_Y * b->VEC_Y;
}

void vec2_div(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X / b->VEC_X;
    dst->VEC_Y = a->VEC_Y / b->VEC_Y;
}

f32 vec2_len(const vec2_t *vec) {
    assert(vec);

    return sqrtf(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y);
}

void vec2_norm(vec2_t *dst, const vec2_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec2_len(src);

    if (len == 0.0f) {
        dst->VEC_X = 0.0f;
        dst->VEC_Y = 0.0f;
        return;
    }

    dst->VEC_X = src->VEC_X / len;
    dst->VEC_Y = src->VEC_Y / len;
}

f32 vec2_dot(const vec2_t *a, const vec2_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y;
}

void vec3_add(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X + b->VEC_X;
    dst->VEC_Y = a->VEC_Y + b->VEC_Y;
    dst->VEC_Z = a->VEC_Z + b->VEC_Z;
}

void vec3_sub(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X - b->VEC_X;
    dst->VEC_Y = a->VEC_Y - b->VEC_Y;
    dst->VEC_Z = a->VEC_Z - b->VEC_Z;
}

void vec3_mul(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X * b->VEC_X;
    dst->VEC_Y = a->VEC_Y * b->VEC_Y;
    dst->VEC_Z = a->VEC_Z * b->VEC_Z;
}

void vec3_div(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X / b->VEC_X;
    dst->VEC_Y = a->VEC_Y / b->VEC_Y;
    dst->VEC_Z = a->VEC_Z / b->VEC_Z;
}

f32 vec3_len(const vec3_t *vec) {
    assert(vec);

    return sqrtf(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y +
                 vec->VEC_Z * vec->VEC_Z);
}

void vec3_norm(vec3_t *dst, const vec3_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec3_len(src);

    if (len == 0.0f) {
        dst->VEC_X = 0.0f;
        dst->VEC_Y = 0.0f;
        dst->VEC_Z = 0.0f;
        return;
    }

    dst->VEC_X = src->VEC_X / len;
    dst->VEC_Y = src->VEC_Y / len;
    dst->VEC_Z = src->VEC_Z / len;
}

f32 vec3_dot(const vec3_t *a, const vec3_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z;
}

void vec3_cross(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_Y * b->VEC_Z - a->VEC_Z * b->VEC_Y;
    dst->VEC_Y = a->VEC_Z * b->VEC_X - a->VEC_X * b->VEC_Z;
    dst->VEC_Z = a->VEC_X * b->VEC_Y - a->VEC_Y * b->VEC_X;
}

void vec4_add(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X + b->VEC_X;
    dst->VEC_Y = a->VEC_Y + b->VEC_Y;
    dst->VEC_Z = a->VEC_Z + b->VEC_Z;
    dst->VEC_W = a->VEC_W + b->VEC_W;
}

void vec4_sub(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X - b->VEC_X;
    dst->VEC_Y = a->VEC_Y - b->VEC_Y;
    dst->VEC_Z = a->VEC_Z - b->VEC_Z;
    dst->VEC_W = a->VEC_W - b->VEC_W;
}

void vec4_mul(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X * b->VEC_X;
    dst->VEC_Y = a->VEC_Y * b->VEC_Y;
    dst->VEC_Z = a->VEC_Z * b->VEC_Z;
    dst->VEC_W = a->VEC_W * b->VEC_W;
}

void vec4_div(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->VEC_X = a->VEC_X / b->VEC_X;
    dst->VEC_Y = a->VEC_Y / b->VEC_Y;
    dst->VEC_Z = a->VEC_Z / b->VEC_Z;
    dst->VEC_W = a->VEC_W / b->VEC_W;
}

f32 vec4_len(const vec4_t *vec) {
    assert(vec);

    return sqrtf(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y +
                 vec->VEC_Z * vec->VEC_Z + vec->VEC_W * vec->VEC_W);
}

void vec4_norm(vec4_t *dst, const vec4_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec4_len(src);

    if (len == 0.0f) {
        dst->VEC_X = 0.0f;
        dst->VEC_Y = 0.0f;
        dst->VEC_Z = 0.0f;
        dst->VEC_W = 0.0f;
        return;
    }

    dst->VEC_X = src->VEC_X / len;
    dst->VEC_Y = src->VEC_Y / len;
    dst->VEC_Z = src->VEC_Z / len;
    dst->VEC_W = src->VEC_W / len;
}

f32 vec4_dot(const vec4_t *a, const vec4_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z +
           a->VEC_W * b->VEC_W;
}
