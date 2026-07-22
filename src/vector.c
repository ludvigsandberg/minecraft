#include "vector.h"

#include <math.h>
#include <assert.h>

vec2_t vec2_add(const vec2_t *a, const vec2_t *b) {
    vec2_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;

    return result;
}

vec2_t vec2_sub(const vec2_t *a, const vec2_t *b) {
    vec2_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;

    return result;
}

vec2_t vec2_mul(const vec2_t *a, const vec2_t *b) {
    vec2_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;

    return result;
}

vec2_t vec2_div(const vec2_t *a, const vec2_t *b) {
    vec2_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;

    return result;
}

float vec2_len(const vec2_t *vec) {
    assert(vec);

    return (float)sqrt(
        (double)(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y));
}

vec2_t vec2_norm(const vec2_t *src) {
    vec2_t result;
    float len;

    assert(src);

    len = vec2_len(src);

    if (len < 1e-6f) {
        result.VEC_X = 0.0f;
        result.VEC_Y = 0.0f;
        return result;
    }

    result.VEC_X = src->VEC_X / len;
    result.VEC_Y = src->VEC_Y / len;

    return result;
}

float vec2_dot(const vec2_t *a, const vec2_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y;
}

vec3_t vec3_add(const vec3_t *a, const vec3_t *b) {
    vec3_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;
    result.VEC_Z = a->VEC_Z + b->VEC_Z;

    return result;
}

vec3_t vec3_sub(const vec3_t *a, const vec3_t *b) {
    vec3_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;
    result.VEC_Z = a->VEC_Z - b->VEC_Z;

    return result;
}

vec3_t vec3_mul(const vec3_t *a, const vec3_t *b) {
    vec3_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;
    result.VEC_Z = a->VEC_Z * b->VEC_Z;

    return result;
}

vec3_t vec3_div(const vec3_t *a, const vec3_t *b) {
    vec3_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;
    result.VEC_Z = a->VEC_Z / b->VEC_Z;

    return result;
}

float vec3_len(const vec3_t *vec) {
    assert(vec);

    return (float)sqrt((double)(vec->VEC_X * vec->VEC_X +
                                vec->VEC_Y * vec->VEC_Y +
                                vec->VEC_Z * vec->VEC_Z));
}

vec3_t vec3_norm(const vec3_t *src) {
    vec3_t result;
    float len;

    assert(src);

    len = vec3_len(src);

    if (len < 1e-6f) {
        result.VEC_X = 0.0f;
        result.VEC_Y = 0.0f;
        result.VEC_Z = 0.0f;
        return result;
    }

    result.VEC_X = src->VEC_X / len;
    result.VEC_Y = src->VEC_Y / len;
    result.VEC_Z = src->VEC_Z / len;

    return result;
}

float vec3_dot(const vec3_t *a, const vec3_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z;
}

vec3_t vec3_cross(const vec3_t *a, const vec3_t *b) {
    vec3_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_Y * b->VEC_Z - a->VEC_Z * b->VEC_Y;
    result.VEC_Y = a->VEC_Z * b->VEC_X - a->VEC_X * b->VEC_Z;
    result.VEC_Z = a->VEC_X * b->VEC_Y - a->VEC_Y * b->VEC_X;

    return result;
}

vec4_t vec4_add(const vec4_t *a, const vec4_t *b) {
    vec4_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;
    result.VEC_Z = a->VEC_Z + b->VEC_Z;
    result.VEC_W = a->VEC_W + b->VEC_W;

    return result;
}

vec4_t vec4_sub(const vec4_t *a, const vec4_t *b) {
    vec4_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;
    result.VEC_Z = a->VEC_Z - b->VEC_Z;
    result.VEC_W = a->VEC_W - b->VEC_W;

    return result;
}

vec4_t vec4_mul(const vec4_t *a, const vec4_t *b) {
    vec4_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;
    result.VEC_Z = a->VEC_Z * b->VEC_Z;
    result.VEC_W = a->VEC_W * b->VEC_W;

    return result;
}

vec4_t vec4_div(const vec4_t *a, const vec4_t *b) {
    vec4_t result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;
    result.VEC_Z = a->VEC_Z / b->VEC_Z;
    result.VEC_W = a->VEC_W / b->VEC_W;

    return result;
}

float vec4_len(const vec4_t *vec) {
    assert(vec);

    return (float)sqrt(
        (double)(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y +
                 vec->VEC_Z * vec->VEC_Z + vec->VEC_W * vec->VEC_W));
}

vec4_t vec4_norm(const vec4_t *src) {
    vec4_t result;
    float len;

    assert(src);

    len = vec4_len(src);

    if (len < 1e-6f) {
        result.VEC_X = 0.0f;
        result.VEC_Y = 0.0f;
        result.VEC_Z = 0.0f;
        result.VEC_W = 0.0f;
        return result;
    }

    result.VEC_X = src->VEC_X / len;
    result.VEC_Y = src->VEC_Y / len;
    result.VEC_Z = src->VEC_Z / len;
    result.VEC_W = src->VEC_W / len;

    return result;
}

float vec4_dot(const vec4_t *a, const vec4_t *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z +
           a->VEC_W * b->VEC_W;
}
