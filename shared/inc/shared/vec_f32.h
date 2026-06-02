/* This file was generated. Do not edit. */

#ifndef SHARED_VEC_F32_H
#define SHARED_VEC_F32_H

#include "shared/types.h"

typedef union vec2_f32_u {
    struct {
        f32 x;
        f32 y;
    } pos;
    struct {
        f32 u;
        f32 v;
    } uv;
    f32 elems[2];
} vec2_f32_t;

typedef union vec3_f32_u {
    struct {
        f32 x;
        f32 y;
        f32 z;
    } pos;
    struct {
        f32 r;
        f32 g;
        f32 b;
    } col;
    struct {
        f32 u;
        f32 v;
        f32 w;
    } uv;
    f32 elems[3];
} vec3_f32_t;

typedef union vec4_f32_u {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    } pos;
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    } col;
    struct {
        f32 u;
        f32 v;
        f32 w;
        f32 q;
    } uv;
    f32 elems[4];
} vec4_f32_t;

void vec2_f32_add(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b);
void vec2_f32_sub(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b);
void vec2_f32_mul(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b);
void vec2_f32_div(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b);
f32 vec2_f32_len(const vec2_f32_t *vec);
void vec2_f32_norm(vec2_f32_t *dst, const vec2_f32_t *src);
void vec2_f32_dot(vec2_f32_t *dst, const vec2_f32_t *a, const vec2_f32_t *b);

void vec3_f32_add(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b);
void vec3_f32_sub(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b);
void vec3_f32_mul(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b);
void vec3_f32_div(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b);
f32 vec3_f32_len(const vec3_f32_t *vec);
void vec3_f32_norm(vec3_f32_t *dst, const vec3_f32_t *src);
void vec3_f32_dot(vec3_f32_t *dst, const vec3_f32_t *a, const vec3_f32_t *b);

void vec4_f32_add(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b);
void vec4_f32_sub(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b);
void vec4_f32_mul(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b);
void vec4_f32_div(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b);
f32 vec4_f32_len(const vec4_f32_t *vec);
void vec4_f32_norm(vec4_f32_t *dst, const vec4_f32_t *src);
void vec4_f32_dot(vec4_f32_t *dst, const vec4_f32_t *a, const vec4_f32_t *b);

#endif
