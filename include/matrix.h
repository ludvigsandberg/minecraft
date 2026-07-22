#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

#define MAT3X3_AT(COL, ROW) elems[(COL) * 3 + (ROW)]
#define MAT4X4_AT(COL, ROW) elems[(COL) * 4 + (ROW)]

#define MAT3X3_IDENTITY                                                       \
    {{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}}
#define MAT4X4_IDENTITY                                                       \
    {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, \
      0.0f, 0.0f, 0.0f, 1.0f}}

typedef struct mat3x3_s {
    float elems[9];
} mat3x3_t;

typedef struct mat4x4_s {
    float elems[16];
} mat4x4_t;

mat3x3_t mat3x3_mul(const mat3x3_t *a, const mat3x3_t *b);

mat4x4_t mat4x4_mul(const mat4x4_t *a, const mat4x4_t *b);
mat4x4_t mat4x4_translate(const mat4x4_t *mat, const vec3_t *pos);
mat4x4_t mat4x4_rotate_x(const mat4x4_t *mat, float angle);
mat4x4_t mat4x4_rotate_y(const mat4x4_t *mat, float angle);
mat4x4_t mat4x4_rotate_z(const mat4x4_t *mat, float angle);
mat4x4_t mat4x4_scale(const mat4x4_t *mat, const vec3_t *scale);

/* Quick inversion of view matrix. */
mat4x4_t invert_view(const mat4x4_t *view);
mat4x4_t look_at(const vec3_t *pos, const vec3_t *target, const vec3_t *up);
mat4x4_t perspective(float aspect, float fov, float near, float far);

#endif
