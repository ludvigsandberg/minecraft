#ifndef VECTOR_H
#define VECTOR_H

#define VEC_X elems[0]
#define VEC_Y elems[1]
#define VEC_Z elems[2]
#define VEC_W elems[3]
#define VEC_R elems[0]
#define VEC_G elems[1]
#define VEC_B elems[2]
#define VEC_A elems[3]
#define VEC_U elems[0]
#define VEC_V elems[1]
#define VEC_S elems[2]
#define VEC_T elems[3]

struct vector2 {
    float elems[2];
};

struct vector3 {
    float elems[3];
};

struct vector4 {
    float elems[4];
};

struct vector2 vector2_add(const struct vector2 *a, const struct vector2 *b);
struct vector2 vector2_sub(const struct vector2 *a, const struct vector2 *b);
struct vector2 vector2_mul(const struct vector2 *a, const struct vector2 *b);
struct vector2 vector2_div(const struct vector2 *a, const struct vector2 *b);
float vector2_len(const struct vector2 *vector);
struct vector2 vector2_norm(const struct vector2 *vector);
float vector2_dot(const struct vector2 *a, const struct vector2 *b);

struct vector3 vector3_add(const struct vector3 *a, const struct vector3 *b);
struct vector3 vector3_sub(const struct vector3 *a, const struct vector3 *b);
struct vector3 vector3_mul(const struct vector3 *a, const struct vector3 *b);
struct vector3 vector3_div(const struct vector3 *a, const struct vector3 *b);
float vector3_len(const struct vector3 *vector);
struct vector3 vector3_norm(const struct vector3 *vector);
float vector3_dot(const struct vector3 *a, const struct vector3 *b);
struct vector3 vector3_cross(const struct vector3 *a, const struct vector3 *b);

struct vector4 vector4_add(const struct vector4 *a, const struct vector4 *b);
struct vector4 vector4_sub(const struct vector4 *a, const struct vector4 *b);
struct vector4 vector4_mul(const struct vector4 *a, const struct vector4 *b);
struct vector4 vector4_div(const struct vector4 *a, const struct vector4 *b);
float vector4_len(const struct vector4 *vector);
struct vector4 vector4_norm(const struct vector4 *vector);
float vector4_dot(const struct vector4 *a, const struct vector4 *b);

#endif
