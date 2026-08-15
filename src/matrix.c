#include "matrix.h"

#include <math.h>
#include <string.h>
#include <assert.h>

struct matrix4 mat4x4_mul(const struct matrix4 *a, const struct matrix4 *b) {
    struct matrix4 result;
    int col;
    int row;
    int i;

    assert(a);
    assert(b);

    for (col = 0; col < 4; col++) {
        for (row = 0; row < 4; row++) {
            result.MATRIX4_AT(col, row) = 0.0f;

            for (i = 0; i < 4; i++) {
                result.MATRIX4_AT(col, row) +=
                    a->MATRIX4_AT(i, row) * b->MATRIX4_AT(col, i);
            }
        }
    }

    return result;
}

struct matrix4 mat4x4_translate(const struct matrix4 *mat,
                                const struct vector3 *pos) {
    struct matrix4 m = MATRIX4_IDENTITY;

    assert(mat);
    assert(pos);

    m.MATRIX4_AT(3, 0) = pos->VEC_X;
    m.MATRIX4_AT(3, 1) = pos->VEC_Y;
    m.MATRIX4_AT(3, 2) = pos->VEC_Z;

    return mat4x4_mul(mat, &m);
}

struct matrix4 mat4x4_rotate_x(const struct matrix4 *mat, float angle) {
    struct matrix4 rot = MATRIX4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MATRIX4_AT(1, 1) = c;
    rot.MATRIX4_AT(2, 1) = s;
    rot.MATRIX4_AT(1, 2) = -s;
    rot.MATRIX4_AT(2, 2) = c;

    return mat4x4_mul(mat, &rot);
}

struct matrix4 mat4x4_rotate_y(const struct matrix4 *mat, float angle) {
    struct matrix4 rot = MATRIX4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MATRIX4_AT(0, 0) = c;
    rot.MATRIX4_AT(2, 0) = -s;
    rot.MATRIX4_AT(0, 2) = s;
    rot.MATRIX4_AT(2, 2) = c;

    return mat4x4_mul(mat, &rot);
}

struct matrix4 mat4x4_rotate_z(const struct matrix4 *mat, float angle) {
    struct matrix4 rot = MATRIX4_IDENTITY;
    float c;
    float s;

    assert(mat);

    c = (float)cos((double)angle);
    s = (float)sin((double)angle);

    rot.MATRIX4_AT(0, 0) = c;
    rot.MATRIX4_AT(1, 0) = s;
    rot.MATRIX4_AT(0, 1) = -s;
    rot.MATRIX4_AT(1, 1) = c;

    return mat4x4_mul(mat, &rot);
}

struct matrix4 mat4x4_scale(const struct matrix4 *mat,
                            const struct vector3 *scale) {
    struct matrix4 s = MATRIX4_IDENTITY;

    assert(mat);
    assert(scale);

    s.MATRIX4_AT(0, 0) = scale->VEC_X;
    s.MATRIX4_AT(1, 1) = scale->VEC_Y;
    s.MATRIX4_AT(2, 2) = scale->VEC_Z;

    return mat4x4_mul(mat, &s);
}

struct matrix4 invert_view(const struct matrix4 *view) {
    struct matrix4 result;
    int i;
    int j;

    assert(view);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result.MATRIX4_AT(i, j) = view->MATRIX4_AT(j, i);
        }
    }

    for (i = 0; i < 3; i++) {
        result.MATRIX4_AT(3, i) =
            -(result.MATRIX4_AT(0, i) * view->MATRIX4_AT(3, 0) +
              result.MATRIX4_AT(1, i) * view->MATRIX4_AT(3, 1) +
              result.MATRIX4_AT(2, i) * view->MATRIX4_AT(3, 2));
    }

    result.MATRIX4_AT(0, 3) = 0.0f;
    result.MATRIX4_AT(1, 3) = 0.0f;
    result.MATRIX4_AT(2, 3) = 0.0f;
    result.MATRIX4_AT(3, 3) = 1.0f;

    return result;
}

struct matrix4 look_at(const struct vector3 *pos, const struct vector3 *target,
                       const struct vector3 *up) {
    struct matrix4 result;
    struct vector3 diff;
    struct vector3 forward;
    struct vector3 right;
    struct vector3 local_up;

    assert(pos);
    assert(target);
    assert(up);

    diff     = vector3_sub(target, pos);
    forward  = vector3_norm(&diff);
    right    = vector3_cross(&forward, up);
    right    = vector3_norm(&right);
    local_up = vector3_cross(&right, &forward);
    local_up = vector3_norm(&local_up);

    result.MATRIX4_AT(0, 0) = right.VEC_X;
    result.MATRIX4_AT(1, 0) = right.VEC_Y;
    result.MATRIX4_AT(2, 0) = right.VEC_Z;
    result.MATRIX4_AT(3, 0) = -vector3_dot(&right, pos);
    result.MATRIX4_AT(0, 1) = local_up.VEC_X;
    result.MATRIX4_AT(1, 1) = local_up.VEC_Y;
    result.MATRIX4_AT(2, 1) = local_up.VEC_Z;
    result.MATRIX4_AT(3, 1) = -vector3_dot(&local_up, pos);
    result.MATRIX4_AT(0, 2) = -forward.VEC_X;
    result.MATRIX4_AT(1, 2) = -forward.VEC_Y;
    result.MATRIX4_AT(2, 2) = -forward.VEC_Z;
    result.MATRIX4_AT(3, 2) = vector3_dot(&forward, pos);
    result.MATRIX4_AT(0, 3) = 0.0f;
    result.MATRIX4_AT(1, 3) = 0.0f;
    result.MATRIX4_AT(2, 3) = 0.0f;
    result.MATRIX4_AT(3, 3) = 1.0f;

    return result;
}

struct matrix4 perspective(float aspect, float fov, float near, float far) {
    struct matrix4 result;
    float half_tan;
    float diff;

    half_tan = (float)tan((double)(fov / 2.0f));
    diff     = far - near;

    result.MATRIX4_AT(0, 0) = 1.0f / (half_tan * aspect);
    result.MATRIX4_AT(1, 0) = 0.0f;
    result.MATRIX4_AT(2, 0) = 0.0f;
    result.MATRIX4_AT(3, 0) = 0.0f;
    result.MATRIX4_AT(0, 1) = 0.0f;
    result.MATRIX4_AT(1, 1) = 1.0f / half_tan;
    result.MATRIX4_AT(2, 1) = 0.0f;
    result.MATRIX4_AT(3, 1) = 0.0f;
    result.MATRIX4_AT(0, 2) = 0.0f;
    result.MATRIX4_AT(1, 2) = 0.0f;
    result.MATRIX4_AT(2, 2) = -(far + near) / diff;
    result.MATRIX4_AT(3, 2) = -(2.0f * far * near) / diff;
    result.MATRIX4_AT(0, 3) = 0.0f;
    result.MATRIX4_AT(1, 3) = 0.0f;
    result.MATRIX4_AT(2, 3) = -1.0f;
    result.MATRIX4_AT(3, 3) = 0.0f;

    return result;
}