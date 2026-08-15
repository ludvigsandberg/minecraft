#include "vector.h"

#include <math.h>
#include <assert.h>

struct vector2 vector2_add(const struct vector2 *a, const struct vector2 *b) {
    struct vector2 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;

    return result;
}

struct vector2 vector2_sub(const struct vector2 *a, const struct vector2 *b) {
    struct vector2 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;

    return result;
}

struct vector2 vector2_mul(const struct vector2 *a, const struct vector2 *b) {
    struct vector2 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;

    return result;
}

struct vector2 vector2_div(const struct vector2 *a, const struct vector2 *b) {
    struct vector2 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;

    return result;
}

float vector2_len(const struct vector2 *vec) {
    assert(vec);

    return (float)sqrt(
        (double)(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y));
}

struct vector2 vector2_norm(const struct vector2 *src) {
    struct vector2 result;
    float len;

    assert(src);

    len = vector2_len(src);

    if (len < 1e-6f) {
        result.VEC_X = 0.0f;
        result.VEC_Y = 0.0f;
        return result;
    }

    result.VEC_X = src->VEC_X / len;
    result.VEC_Y = src->VEC_Y / len;

    return result;
}

float vector2_dot(const struct vector2 *a, const struct vector2 *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y;
}

struct vector3 vector3_add(const struct vector3 *a, const struct vector3 *b) {
    struct vector3 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;
    result.VEC_Z = a->VEC_Z + b->VEC_Z;

    return result;
}

struct vector3 vector3_sub(const struct vector3 *a, const struct vector3 *b) {
    struct vector3 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;
    result.VEC_Z = a->VEC_Z - b->VEC_Z;

    return result;
}

struct vector3 vector3_mul(const struct vector3 *a, const struct vector3 *b) {
    struct vector3 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;
    result.VEC_Z = a->VEC_Z * b->VEC_Z;

    return result;
}

struct vector3 vector3_div(const struct vector3 *a, const struct vector3 *b) {
    struct vector3 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;
    result.VEC_Z = a->VEC_Z / b->VEC_Z;

    return result;
}

float vector3_len(const struct vector3 *vec) {
    assert(vec);

    return (float)sqrt((double)(vec->VEC_X * vec->VEC_X +
                                vec->VEC_Y * vec->VEC_Y +
                                vec->VEC_Z * vec->VEC_Z));
}

struct vector3 vector3_norm(const struct vector3 *src) {
    struct vector3 result;
    float len;

    assert(src);

    len = vector3_len(src);

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

float vector3_dot(const struct vector3 *a, const struct vector3 *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z;
}

struct vector3 vector3_cross(const struct vector3 *a,
                             const struct vector3 *b) {
    struct vector3 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_Y * b->VEC_Z - a->VEC_Z * b->VEC_Y;
    result.VEC_Y = a->VEC_Z * b->VEC_X - a->VEC_X * b->VEC_Z;
    result.VEC_Z = a->VEC_X * b->VEC_Y - a->VEC_Y * b->VEC_X;

    return result;
}

struct vector4 vector4_add(const struct vector4 *a, const struct vector4 *b) {
    struct vector4 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X + b->VEC_X;
    result.VEC_Y = a->VEC_Y + b->VEC_Y;
    result.VEC_Z = a->VEC_Z + b->VEC_Z;
    result.VEC_W = a->VEC_W + b->VEC_W;

    return result;
}

struct vector4 vector4_sub(const struct vector4 *a, const struct vector4 *b) {
    struct vector4 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X - b->VEC_X;
    result.VEC_Y = a->VEC_Y - b->VEC_Y;
    result.VEC_Z = a->VEC_Z - b->VEC_Z;
    result.VEC_W = a->VEC_W - b->VEC_W;

    return result;
}

struct vector4 vector4_mul(const struct vector4 *a, const struct vector4 *b) {
    struct vector4 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X * b->VEC_X;
    result.VEC_Y = a->VEC_Y * b->VEC_Y;
    result.VEC_Z = a->VEC_Z * b->VEC_Z;
    result.VEC_W = a->VEC_W * b->VEC_W;

    return result;
}

struct vector4 vector4_div(const struct vector4 *a, const struct vector4 *b) {
    struct vector4 result;

    assert(a);
    assert(b);

    result.VEC_X = a->VEC_X / b->VEC_X;
    result.VEC_Y = a->VEC_Y / b->VEC_Y;
    result.VEC_Z = a->VEC_Z / b->VEC_Z;
    result.VEC_W = a->VEC_W / b->VEC_W;

    return result;
}

float vector4_len(const struct vector4 *vec) {
    assert(vec);

    return (float)sqrt(
        (double)(vec->VEC_X * vec->VEC_X + vec->VEC_Y * vec->VEC_Y +
                 vec->VEC_Z * vec->VEC_Z + vec->VEC_W * vec->VEC_W));
}

struct vector4 vector4_norm(const struct vector4 *src) {
    struct vector4 result;
    float len;

    assert(src);

    len = vector4_len(src);

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

float vector4_dot(const struct vector4 *a, const struct vector4 *b) {
    assert(a);
    assert(b);

    return a->VEC_X * b->VEC_X + a->VEC_Y * b->VEC_Y + a->VEC_Z * b->VEC_Z +
           a->VEC_W * b->VEC_W;
}
