/*****************************************************************************
 * File:        vec.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_VEC_H
#define COMMON_VEC_H

#include "common/types.h"

#define VEC_X(V) (V).elems[0]
#define VEC_Y(V) (V).elems[1]
#define VEC_Z(V) (V).elems[2]
#define VEC_W(V) (V).elems[3]
#define VEC_R(V) (V).elems[0]
#define VEC_G(V) (V).elems[1]
#define VEC_B(V) (V).elems[2]
#define VEC_A(V) (V).elems[3]
#define VEC_U(V) (V).elems[0]
#define VEC_V(V) (V).elems[1]
#define VEC_S(V) (V).elems[2]
#define VEC_T(V) (V).elems[3]

typedef struct vec2_u {
    f32 elems[2];
} vec2_t;

typedef struct vec3_u {
    f32 elems[3];
} vec3_t;

typedef struct vec4_u {
    f32 elems[4];
} vec4_t;

void vec2_add(vec2_t *dst, const vec2_t *a, const vec2_t *b);
void vec2_sub(vec2_t *dst, const vec2_t *a, const vec2_t *b);
void vec2_mul(vec2_t *dst, const vec2_t *a, const vec2_t *b);
void vec2_div(vec2_t *dst, const vec2_t *a, const vec2_t *b);
f32 vec2_len(const vec2_t *vec);
void vec2_norm(vec2_t *dst, const vec2_t *src);
void vec2_dot(vec2_t *dst, const vec2_t *a, const vec2_t *b);

void vec3_add(vec3_t *dst, const vec3_t *a, const vec3_t *b);
void vec3_sub(vec3_t *dst, const vec3_t *a, const vec3_t *b);
void vec3_mul(vec3_t *dst, const vec3_t *a, const vec3_t *b);
void vec3_div(vec3_t *dst, const vec3_t *a, const vec3_t *b);
f32 vec3_len(const vec3_t *vec);
void vec3_norm(vec3_t *dst, const vec3_t *src);
void vec3_dot(vec3_t *dst, const vec3_t *a, const vec3_t *b);

void vec4_add(vec4_t *dst, const vec4_t *a, const vec4_t *b);
void vec4_sub(vec4_t *dst, const vec4_t *a, const vec4_t *b);
void vec4_mul(vec4_t *dst, const vec4_t *a, const vec4_t *b);
void vec4_div(vec4_t *dst, const vec4_t *a, const vec4_t *b);
f32 vec4_len(const vec4_t *vec);
void vec4_norm(vec4_t *dst, const vec4_t *src);
void vec4_dot(vec4_t *dst, const vec4_t *a, const vec4_t *b);

#endif
