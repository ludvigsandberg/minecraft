/*****************************************************************************
 * File:        mat.c
 * Author:      ludvigsandberg
 * Date:        2026-07-12
 * Description: -
 *****************************************************************************/

#include "common/mat.h"

#include <math.h>
#include <assert.h>

void mat3x3_mul(mat3x3_t *dst, const mat3x3_t *a, const mat3x3_t *b) {
    int col;
    int row;
    int i;

    assert(dst);
    assert(a);
    assert(b);
    assert(dst != a && dst != b);

    for (col = 0; col < 3; col++) {
        for (row = 0; row < 3; row++) {
            dst->MAT4X4_AT(col, row) = 0.0f;

            for (i = 0; i < 3; i++) {
                dst->MAT4X4_AT(col, row) +=
                    a->MAT4X4_AT(i, row) * b->MAT4X4_AT(col, i);
            }
        }
    }
}

void mat4x4_mul(mat4x4_t *dst, const mat4x4_t *a, const mat4x4_t *b) {
    int col;
    int row;
    int i;

    assert(dst);
    assert(a);
    assert(b);
    assert(dst != a && dst != b);

    for (col = 0; col < 4; col++) {
        for (row = 0; row < 4; row++) {
            dst->MAT4X4_AT(col, row) = 0.0f;

            for (i = 0; i < 4; i++) {
                dst->MAT4X4_AT(col, row) +=
                    a->MAT4X4_AT(i, row) * b->MAT4X4_AT(col, i);
            }
        }
    }
}

void mat4x4_translate(mat4x4_t *dst, const mat4x4_t *mat,
                      const vec3_t *offset) {
    mat4x4_t m = MAT4X4_IDENTITY;

    assert(dst);
    assert(mat);
    assert(offset);
    assert(dst != mat);

    m.MAT4X4_AT(3, 0) = offset->VEC_X;
    m.MAT4X4_AT(3, 1) = offset->VEC_Y;
    m.MAT4X4_AT(3, 2) = offset->VEC_Z;

    mat4x4_mul(dst, mat, &m);
}

void invert_view(mat4x4_t *dst, const mat4x4_t *view) {
    int i;
    int j;

    assert(dst);
    assert(view);
    assert(dst != view);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            dst->MAT4X4_AT(i, j) = view->MAT4X4_AT(j, i);
        }
    }

    for (i = 0; i < 3; i++) {
        dst->MAT4X4_AT(3, i) = -(dst->MAT4X4_AT(0, i) * view->MAT4X4_AT(3, 0) +
                                 dst->MAT4X4_AT(1, i) * view->MAT4X4_AT(3, 1) +
                                 dst->MAT4X4_AT(2, i) * view->MAT4X4_AT(3, 2));
    }

    dst->MAT4X4_AT(0, 3) = 0.0f;
    dst->MAT4X4_AT(1, 3) = 0.0f;
    dst->MAT4X4_AT(2, 3) = 0.0f;
    dst->MAT4X4_AT(3, 3) = 1.0f;
}

void look_at(mat4x4_t *dst, const vec3_t *pos, const vec3_t *target,
             const vec3_t *up) {
    vec3_t diff;
    vec3_t forward;
    vec3_t right;
    vec3_t local_up;

    assert(dst);
    assert(pos);
    assert(target);
    assert(up);

    vec3_sub(&diff, target, pos);
    vec3_norm(&forward, &diff);
    vec3_cross(&right, &forward, up);
    vec3_norm(&right, &right);
    vec3_cross(&local_up, &right, &forward);
    vec3_norm(&local_up, &local_up);

    dst->MAT4X4_AT(0, 0) = right.VEC_X;
    dst->MAT4X4_AT(1, 0) = right.VEC_Y;
    dst->MAT4X4_AT(2, 0) = right.VEC_Z;
    dst->MAT4X4_AT(3, 0) = -vec3_dot(&right, pos);
    dst->MAT4X4_AT(0, 1) = local_up.VEC_X;
    dst->MAT4X4_AT(1, 1) = local_up.VEC_Y;
    dst->MAT4X4_AT(2, 1) = local_up.VEC_Z;
    dst->MAT4X4_AT(3, 1) = -vec3_dot(&local_up, pos);
    dst->MAT4X4_AT(0, 2) = -forward.VEC_X;
    dst->MAT4X4_AT(1, 2) = -forward.VEC_Y;
    dst->MAT4X4_AT(2, 2) = -forward.VEC_Z;
    dst->MAT4X4_AT(3, 2) = vec3_dot(&forward, pos);
    dst->MAT4X4_AT(0, 3) = 0.0f;
    dst->MAT4X4_AT(1, 3) = 0.0f;
    dst->MAT4X4_AT(2, 3) = 0.0f;
    dst->MAT4X4_AT(3, 3) = 1.0f;
}

void perspective(mat4x4_t *dst, f32 aspect, f32 fov, f32 near, f32 far) {
    f32 half_tan;
    f32 diff;

    assert(dst);

    half_tan = tanf(fov / 2.0f);
    diff     = far - near;

    dst->MAT4X4_AT(0, 0) = 1.0f / (half_tan * aspect);
    dst->MAT4X4_AT(1, 0) = 0.0f;
    dst->MAT4X4_AT(2, 0) = 0.0f;
    dst->MAT4X4_AT(3, 0) = 0.0f;
    dst->MAT4X4_AT(0, 1) = 0.0f;
    dst->MAT4X4_AT(1, 1) = 1.0f / half_tan;
    dst->MAT4X4_AT(2, 1) = 0.0f;
    dst->MAT4X4_AT(3, 1) = 0.0f;
    dst->MAT4X4_AT(0, 2) = 0.0f;
    dst->MAT4X4_AT(1, 2) = 0.0f;
    dst->MAT4X4_AT(2, 2) = -(far + near) / diff;
    dst->MAT4X4_AT(3, 2) = -(2.0f * far * near) / diff;
    dst->MAT4X4_AT(0, 3) = 0.0f;
    dst->MAT4X4_AT(1, 3) = 0.0f;
    dst->MAT4X4_AT(2, 3) = -1.0f;
    dst->MAT4X4_AT(3, 3) = 0.0f;
}