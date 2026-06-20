#include "common/vec_s32.h"

#include <math.h>
#include <assert.h>

void vec2_s32_add(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
}

void vec2_s32_sub(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
}

void vec2_s32_mul(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
}

void vec2_s32_div(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
}

f32 vec2_s32_len(const vec2_s32_t *vec) {
    assert(vec);

    return (f32)sqrt(
        (double)(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y));
}

void vec2_s32_norm(vec2_s32_t *dst, const vec2_s32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec2_s32_len(src);

    dst->pos.x = (s32)(src->pos.x / len);
    dst->pos.y = (s32)(src->pos.y / len);
}

void vec2_s32_dot(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y;
}

void vec3_s32_add(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
    dst->pos.z = a->pos.z + b->pos.z;
}

void vec3_s32_sub(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
    dst->pos.z = a->pos.z - b->pos.z;
}

void vec3_s32_mul(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
    dst->pos.z = a->pos.z * b->pos.z;
}

void vec3_s32_div(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
    dst->pos.z = a->pos.z / b->pos.z;
}

f32 vec3_s32_len(const vec3_s32_t *vec) {
    assert(vec);

    return (f32)sqrt((double)(vec->pos.x * vec->pos.x +
                              vec->pos.y * vec->pos.y +
                              vec->pos.z * vec->pos.z));
}

void vec3_s32_norm(vec3_s32_t *dst, const vec3_s32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec3_s32_len(src);

    dst->pos.x = (s32)(src->pos.x / len);
    dst->pos.y = (s32)(src->pos.y / len);
    dst->pos.z = (s32)(src->pos.z / len);
}

void vec3_s32_dot(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x =
        a->pos.x * b->pos.x + a->pos.y * b->pos.y + a->pos.z * b->pos.z;
}

void vec4_s32_add(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x + b->pos.x;
    dst->pos.y = a->pos.y + b->pos.y;
    dst->pos.z = a->pos.z + b->pos.z;
    dst->pos.w = a->pos.w + b->pos.w;
}

void vec4_s32_sub(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x - b->pos.x;
    dst->pos.y = a->pos.y - b->pos.y;
    dst->pos.z = a->pos.z - b->pos.z;
    dst->pos.w = a->pos.w - b->pos.w;
}

void vec4_s32_mul(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x;
    dst->pos.y = a->pos.y * b->pos.y;
    dst->pos.z = a->pos.z * b->pos.z;
    dst->pos.w = a->pos.w * b->pos.w;
}

void vec4_s32_div(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x / b->pos.x;
    dst->pos.y = a->pos.y / b->pos.y;
    dst->pos.z = a->pos.z / b->pos.z;
    dst->pos.w = a->pos.w / b->pos.w;
}

f32 vec4_s32_len(const vec4_s32_t *vec) {
    assert(vec);

    return (f32)sqrt(
        (double)(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y +
                 vec->pos.z * vec->pos.z + vec->pos.w * vec->pos.w));
}

void vec4_s32_norm(vec4_s32_t *dst, const vec4_s32_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec4_s32_len(src);

    dst->pos.x = (s32)(src->pos.x / len);
    dst->pos.y = (s32)(src->pos.y / len);
    dst->pos.z = (s32)(src->pos.z / len);
    dst->pos.w = (s32)(src->pos.w / len);
}

void vec4_s32_dot(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y +
                 a->pos.z * b->pos.z + a->pos.w * b->pos.w;
}
