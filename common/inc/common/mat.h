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
    {{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}}
#define MAT4X4_IDENTITY                                                       \
    {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, \
      0.0f, 0.0f, 0.0f, 1.0f}}

#define MAT3X3_AT(COL, ROW) elems[(COL) * 3 + (ROW)]
#define MAT4X4_AT(COL, ROW) elems[(COL) * 4 + (ROW)]

typedef struct mat3x3_s {
    f32 elems[9];
} mat3x3_t;

typedef struct mat4x4_s {
    f32 elems[16];
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
