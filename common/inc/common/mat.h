/*****************************************************************************
 * File:        mat.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_MAT_H
#define COMMON_MAT_H

#include "common/types.h"
#include "common/vec.h"

#define MAT3X3_IDENTITY                                                       \
    {{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f}}
#define MAT4X4_IDENTITY                                                       \
    {{0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, \
      1.0f, 0.0f, 0.0f, 0.0f}}

typedef union mat3x3_u {
    f32 raw[9];
    f32 elems[3][3];

} mat3x3_t;

typedef union mat4x4_u {
    f32 raw[16];
    f32 elems[4][4];
} mat4x4_t;

void mat3x3_mul(mat3x3_t *dst, const mat3x3_t *a, const mat3x3_t *b);

void mat4x4_mul(mat4x4_t *dst, const mat4x4_t *a, const mat4x4_t *b);
void mat4x4_translate(mat4x4_t *dst, const mat4x4_t *mat,
                      const vec3_t *offset);

/* Quick inversion of view matrix. */
void invert_view(mat4x4_t *dst, const mat4x4_t *view);
void look_at(mat4x4_t *dst, const vec3_t *pos, const vec3_t *target,
             const vec3_t *up);
void perspective(mat4x4_t *dst, f32 aspect, f32 fov, f32 near, f32 far);

#endif
