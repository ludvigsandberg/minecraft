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

    VEC_X(*dst) = VEC_X(*a) + VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) + VEC_Y(*b);
}

void vec2_sub(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) - VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) - VEC_Y(*b);
}

void vec2_mul(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) * VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) * VEC_Y(*b);
}

void vec2_div(vec2_t *dst, const vec2_t *a, const vec2_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) / VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) / VEC_Y(*b);
}

f32 vec2_len(const vec2_t *vec) {
    assert(vec);

    return sqrtf(VEC_X(*vec) * VEC_X(*vec) + VEC_Y(*vec) * VEC_Y(*vec));
}

void vec2_norm(vec2_t *dst, const vec2_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec2_len(src);

    if (len == 0.0f) {
        VEC_X(*dst) = 0.0f;
        VEC_Y(*dst) = 0.0f;
        return;
    }

    VEC_X(*dst) = VEC_X(*src) / len;
    VEC_Y(*dst) = VEC_Y(*src) / len;
}

f32 vec2_dot(const vec2_t *a, const vec2_t *b) {
    assert(a);
    assert(b);

    return VEC_X(*a) * VEC_X(*b) + VEC_Y(*a) * VEC_Y(*b);
}

void vec3_add(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) + VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) + VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) + VEC_Z(*b);
}

void vec3_sub(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) - VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) - VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) - VEC_Z(*b);
}

void vec3_mul(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) * VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) * VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) * VEC_Z(*b);
}

void vec3_div(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) / VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) / VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) / VEC_Z(*b);
}

f32 vec3_len(const vec3_t *vec) {
    assert(vec);

    return sqrtf(VEC_X(*vec) * VEC_X(*vec) + VEC_Y(*vec) * VEC_Y(*vec) +
                 VEC_Z(*vec) * VEC_Z(*vec));
}

void vec3_norm(vec3_t *dst, const vec3_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec3_len(src);

    if (len == 0.0f) {
        VEC_X(*dst) = 0.0f;
        VEC_Y(*dst) = 0.0f;
        VEC_Z(*dst) = 0.0f;
        return;
    }

    VEC_X(*dst) = VEC_X(*src) / len;
    VEC_Y(*dst) = VEC_Y(*src) / len;
    VEC_Z(*dst) = VEC_Z(*src) / len;
}

f32 vec3_dot(const vec3_t *a, const vec3_t *b) {
    assert(a);
    assert(b);

    return VEC_X(*a) * VEC_X(*b) + VEC_Y(*a) * VEC_Y(*b) +
           VEC_Z(*a) * VEC_Z(*b);
}

void vec3_cross(vec3_t *dst, const vec3_t *a, const vec3_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_Y(*a) * VEC_Z(*b) - VEC_Z(*a) * VEC_Y(*b);
    VEC_Y(*dst) = VEC_Z(*a) * VEC_X(*b) - VEC_X(*a) * VEC_Z(*b);
    VEC_Z(*dst) = VEC_X(*a) * VEC_Y(*b) - VEC_Y(*a) * VEC_X(*b);
}

void vec4_add(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) + VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) + VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) + VEC_Z(*b);
    VEC_W(*dst) = VEC_W(*a) + VEC_W(*b);
}

void vec4_sub(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) - VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) - VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) - VEC_Z(*b);
    VEC_W(*dst) = VEC_W(*a) - VEC_W(*b);
}

void vec4_mul(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) * VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) * VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) * VEC_Z(*b);
    VEC_W(*dst) = VEC_W(*a) * VEC_W(*b);
}

void vec4_div(vec4_t *dst, const vec4_t *a, const vec4_t *b) {
    assert(dst);
    assert(a);
    assert(b);

    VEC_X(*dst) = VEC_X(*a) / VEC_X(*b);
    VEC_Y(*dst) = VEC_Y(*a) / VEC_Y(*b);
    VEC_Z(*dst) = VEC_Z(*a) / VEC_Z(*b);
    VEC_W(*dst) = VEC_W(*a) / VEC_W(*b);
}

f32 vec4_len(const vec4_t *vec) {
    assert(vec);

    return sqrtf(VEC_X(*vec) * VEC_X(*vec) + VEC_Y(*vec) * VEC_Y(*vec) +
                 VEC_Z(*vec) * VEC_Z(*vec) + VEC_W(*vec) * VEC_W(*vec));
}

void vec4_norm(vec4_t *dst, const vec4_t *src) {
    f32 len;

    assert(dst);
    assert(src);

    len = vec4_len(src);

    if (len == 0.0f) {
        VEC_X(*dst) = 0.0f;
        VEC_Y(*dst) = 0.0f;
        VEC_Z(*dst) = 0.0f;
        VEC_W(*dst) = 0.0f;
        return;
    }

    VEC_X(*dst) = VEC_X(*src) / len;
    VEC_Y(*dst) = VEC_Y(*src) / len;
    VEC_Z(*dst) = VEC_Z(*src) / len;
    VEC_W(*dst) = VEC_W(*src) / len;
}

f32 vec4_dot(const vec4_t *a, const vec4_t *b) {
    assert(a);
    assert(b);

    return VEC_X(*a) * VEC_X(*b) + VEC_Y(*a) * VEC_Y(*b) +
           VEC_Z(*a) * VEC_Z(*b) + VEC_W(*a) * VEC_W(*b);
}
