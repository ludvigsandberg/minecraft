#include "matrix.h"

#include <math.h>
#include <string.h>
#include <assert.h>

mat3x3_t mat3x3_mul(const mat3x3_t *a, const mat3x3_t *b) {
    mat3x3_t result;
    int col;
    int row;
    int i;

    assert(a);
    assert(b);

    for (col = 0; col < 3; col++) {
        for (row = 0; row < 3; row++) {
            result.MAT3X3_AT(col, row) = 0.0f;

            for (i = 0; i < 3; i++) {
                result.MAT3X3_AT(col, row) +=
                    a->MAT3X3_AT(i, row) * b->MAT3X3_AT(col, i);
            }
        }
    }

    return result;
}

mat4x4_t mat4x4_mul(const mat4x4_t *a, const mat4x4_t *b) {
    mat4x4_t result;
    int col;
    int row;
    int i;

    assert(a);
    assert(b);

    for (col = 0; col < 4; col++) {
        for (row = 0; row < 4; row++) {
            result.MAT4X4_AT(col, row) = 0.0f;

            for (i = 0; i < 4; i++) {
                result.MAT4X4_AT(col, row) +=
                    a->MAT4X4_AT(i, row) * b->MAT4X4_AT(col, i);
            }
        }
    }

    return result;
}

mat4x4_t mat4x4_translate(const mat4x4_t *mat, const struct vector3 *pos) {
    mat4x4_t m = MAT4X4_IDENTITY;

    assert(mat);
    assert(pos);

    m.MAT4X4_AT(3, 0) = pos->VEC_X;
    m.MAT4X4_AT(3, 1) = pos->VEC_Y;
    m.MAT4X4_AT(3, 2) = pos->VEC_Z;

    return mat4x4_mul(mat, &m);
}

mat4x4_t mat4x4_rotate_x(const mat4x4_t *mat, float angle) {
    mat4x4_t rot = MAT4X4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MAT4X4_AT(1, 1) = c;
    rot.MAT4X4_AT(2, 1) = s;
    rot.MAT4X4_AT(1, 2) = -s;
    rot.MAT4X4_AT(2, 2) = c;

    return mat4x4_mul(mat, &rot);
}

mat4x4_t mat4x4_rotate_y(const mat4x4_t *mat, float angle) {
    mat4x4_t rot = MAT4X4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MAT4X4_AT(0, 0) = c;
    rot.MAT4X4_AT(2, 0) = -s;
    rot.MAT4X4_AT(0, 2) = s;
    rot.MAT4X4_AT(2, 2) = c;

    return mat4x4_mul(mat, &rot);
}

mat4x4_t mat4x4_rotate_z(const mat4x4_t *mat, float angle) {
    mat4x4_t rot = MAT4X4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MAT4X4_AT(0, 0) = c;
    rot.MAT4X4_AT(1, 0) = s;
    rot.MAT4X4_AT(0, 1) = -s;
    rot.MAT4X4_AT(1, 1) = c;

    return mat4x4_mul(mat, &rot);
}

mat4x4_t mat4x4_scale(const mat4x4_t *mat, const struct vector3 *scale) {
    mat4x4_t s = MAT4X4_IDENTITY;

    assert(mat);
    assert(scale);

    s.MAT4X4_AT(0, 0) = scale->VEC_X;
    s.MAT4X4_AT(1, 1) = scale->VEC_Y;
    s.MAT4X4_AT(2, 2) = scale->VEC_Z;

    return mat4x4_mul(mat, &s);
}

mat4x4_t invert_view(const mat4x4_t *view) {
    mat4x4_t result;
    int i;
    int j;

    assert(view);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result.MAT4X4_AT(i, j) = view->MAT4X4_AT(j, i);
        }
    }

    for (i = 0; i < 3; i++) {
        result.MAT4X4_AT(3, i) =
            -(result.MAT4X4_AT(0, i) * view->MAT4X4_AT(3, 0) +
              result.MAT4X4_AT(1, i) * view->MAT4X4_AT(3, 1) +
              result.MAT4X4_AT(2, i) * view->MAT4X4_AT(3, 2));
    }

    result.MAT4X4_AT(0, 3) = 0.0f;
    result.MAT4X4_AT(1, 3) = 0.0f;
    result.MAT4X4_AT(2, 3) = 0.0f;
    result.MAT4X4_AT(3, 3) = 1.0f;

    return result;
}

mat4x4_t look_at(const struct vector3 *pos, const struct vector3 *target,
                 const struct vector3 *up) {
    mat4x4_t result;
    struct vector3 diff;
    struct vector3 forward;
    struct vector3 right;
    struct vector3 local_up;

    assert(pos);
    assert(target);
    assert(up);

    diff     = vec3_sub(target, pos);
    forward  = vec3_norm(&diff);
    right    = vec3_cross(&forward, up);
    right    = vec3_norm(&right);
    local_up = vec3_cross(&right, &forward);
    local_up = vec3_norm(&local_up);

    result.MAT4X4_AT(0, 0) = right.VEC_X;
    result.MAT4X4_AT(1, 0) = right.VEC_Y;
    result.MAT4X4_AT(2, 0) = right.VEC_Z;
    result.MAT4X4_AT(3, 0) = -vec3_dot(&right, pos);
    result.MAT4X4_AT(0, 1) = local_up.VEC_X;
    result.MAT4X4_AT(1, 1) = local_up.VEC_Y;
    result.MAT4X4_AT(2, 1) = local_up.VEC_Z;
    result.MAT4X4_AT(3, 1) = -vec3_dot(&local_up, pos);
    result.MAT4X4_AT(0, 2) = -forward.VEC_X;
    result.MAT4X4_AT(1, 2) = -forward.VEC_Y;
    result.MAT4X4_AT(2, 2) = -forward.VEC_Z;
    result.MAT4X4_AT(3, 2) = vec3_dot(&forward, pos);
    result.MAT4X4_AT(0, 3) = 0.0f;
    result.MAT4X4_AT(1, 3) = 0.0f;
    result.MAT4X4_AT(2, 3) = 0.0f;
    result.MAT4X4_AT(3, 3) = 1.0f;

    return result;
}

mat4x4_t perspective(float aspect, float fov, float near, float far) {
    mat4x4_t result;
    float half_tan;
    float diff;

    half_tan = (float)tan((double)(fov / 2.0f));
    diff     = far - near;

    result.MAT4X4_AT(0, 0) = 1.0f / (half_tan * aspect);
    result.MAT4X4_AT(1, 0) = 0.0f;
    result.MAT4X4_AT(2, 0) = 0.0f;
    result.MAT4X4_AT(3, 0) = 0.0f;
    result.MAT4X4_AT(0, 1) = 0.0f;
    result.MAT4X4_AT(1, 1) = 1.0f / half_tan;
    result.MAT4X4_AT(2, 1) = 0.0f;
    result.MAT4X4_AT(3, 1) = 0.0f;
    result.MAT4X4_AT(0, 2) = 0.0f;
    result.MAT4X4_AT(1, 2) = 0.0f;
    result.MAT4X4_AT(2, 2) = -(far + near) / diff;
    result.MAT4X4_AT(3, 2) = -(2.0f * far * near) / diff;
    result.MAT4X4_AT(0, 3) = 0.0f;
    result.MAT4X4_AT(1, 3) = 0.0f;
    result.MAT4X4_AT(2, 3) = -1.0f;
    result.MAT4X4_AT(3, 3) = 0.0f;

    return result;
}