/* Generated from shared/inc/shared/vec_s32.h - DO NOT EDIT */

#ifndef SHARED_S32_VEC_H
#define SHARED_S32_VEC_H

#include "shared/types_u32.h"

typedef union vec2_u32_u {
    struct {
        u32 x;
        u32 y;
    } pos;
    struct {
        u32 u;
        u32 v;
    } uv;
    u32 elems[2];
} vec2_u32_t;

typedef union vec3_u32_u {
    struct {
        u32 x;
        u32 y;
        u32 z;
    } pos;
    struct {
        u32 r;
        u32 g;
        u32 b;
    } col;
    struct {
        u32 u;
        u32 v;
        u32 w;
    } uv;
    u32 elems[3];
} vec3_u32_t;

typedef union vec4_u32_u {
    struct {
        u32 x;
        u32 y;
        u32 z;
        u32 w;
    } pos;
    struct {
        u32 r;
        u32 g;
        u32 b;
        u32 a;
    } col;
    struct {
        u32 u;
        u32 v;
        u32 w;
        u32 q;
    } uv;
    u32 elems[4];
} vec4_u32_t;

void vec2_u32_add(vec2_u32_t *dst, const vec2_u32_t *a, const vec2_u32_t *b);
void vec2_u32_sub(vec2_u32_t *dst, const vec2_u32_t *a, const vec2_u32_t *b);
void vec2_u32_mul(vec2_u32_t *dst, const vec2_u32_t *a, const vec2_u32_t *b);
void vec2_u32_div(vec2_u32_t *dst, const vec2_u32_t *a, const vec2_u32_t *b);
f32 vec2_u32_len(const vec2_u32_t *vec);
void vec2_u32_norm(vec2_u32_t *dst, const vec2_u32_t *src);
void vec2_u32_dot(vec2_u32_t *dst, const vec2_u32_t *a, const vec2_u32_t *b);

void vec3_u32_add(vec3_u32_t *dst, const vec3_u32_t *a, const vec3_u32_t *b);
void vec3_u32_sub(vec3_u32_t *dst, const vec3_u32_t *a, const vec3_u32_t *b);
void vec3_u32_mul(vec3_u32_t *dst, const vec3_u32_t *a, const vec3_u32_t *b);
void vec3_u32_div(vec3_u32_t *dst, const vec3_u32_t *a, const vec3_u32_t *b);
f32 vec3_u32_len(const vec3_u32_t *vec);
void vec3_u32_norm(vec3_u32_t *dst, const vec3_u32_t *src);
void vec3_u32_dot(vec3_u32_t *dst, const vec3_u32_t *a, const vec3_u32_t *b);

void vec4_u32_add(vec4_u32_t *dst, const vec4_u32_t *a, const vec4_u32_t *b);
void vec4_u32_sub(vec4_u32_t *dst, const vec4_u32_t *a, const vec4_u32_t *b);
void vec4_u32_mul(vec4_u32_t *dst, const vec4_u32_t *a, const vec4_u32_t *b);
void vec4_u32_div(vec4_u32_t *dst, const vec4_u32_t *a, const vec4_u32_t *b);
f32 vec4_u32_len(const vec4_u32_t *vec);
void vec4_u32_norm(vec4_u32_t *dst, const vec4_u32_t *src);
void vec4_u32_dot(vec4_u32_t *dst, const vec4_u32_t *a, const vec4_u32_t *b);

#endif
