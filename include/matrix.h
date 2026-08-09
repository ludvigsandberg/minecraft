#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

#define MATRIX4_AT(COL, ROW) elems[(COL) * 4 + (ROW)]

#define MATRIX4_IDENTITY                                                      \
    {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, \
      0.0f, 0.0f, 0.0f, 1.0f}}

struct matrix4 {
    float elems[16];
};

struct matrix4 matrix4_mul(const struct matrix4 *a, const struct matrix4 *b);
struct matrix4 matrix4_translate(const struct matrix4 *matrix,
                                 const struct vector3 *pos);
struct matrix4 matrix4_rotate_x(const struct matrix4 *matrix, float angle);
struct matrix4 matrix4_rotate_y(const struct matrix4 *matrix, float angle);
struct matrix4 matrix4_rotate_z(const struct matrix4 *matrix, float angle);
struct matrix4 matrix4_scale(const struct matrix4 *matrix,
                             const struct vector3 *scale);

/* Quick inversion of view matrix. */
struct matrix4 invert_view(const struct matrix4 *view_matrix);
struct matrix4 look_at(const struct vector3 *pos, const struct vector3 *target,
                       const struct vector3 *up);
struct matrix4 perspective(float aspect, float fov, float near, float far);

#endif
