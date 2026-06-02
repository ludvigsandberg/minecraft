#ifndef SHARED_VEC_S32_H
#define SHARED_VEC_S32_H

#include "shared/types.h"

typedef union vec2_s32_u {
    struct {
        s32 x;
        s32 y;
    } pos;
    struct {
        s32 u;
        s32 v;
    } uv;
    s32 elems[2];
} vec2_s32_t;

typedef union vec3_s32_u {
    struct {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    struct {
        s32 r;
        s32 g;
        s32 b;
    } col;
    struct {
        s32 u;
        s32 v;
        s32 w;
    } uv;
    s32 elems[3];
} vec3_s32_t;

typedef union vec4_s32_u {
    struct {
        s32 x;
        s32 y;
        s32 z;
        s32 w;
    } pos;
    struct {
        s32 r;
        s32 g;
        s32 b;
        s32 a;
    } col;
    struct {
        s32 u;
        s32 v;
        s32 w;
        s32 q;
    } uv;
    s32 elems[4];
} vec4_s32_t;

void vec2_s32_add(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b);
void vec2_s32_sub(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b);
void vec2_s32_mul(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b);
void vec2_s32_div(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b);
f32 vec2_s32_len(const vec2_s32_t *vec);
void vec2_s32_norm(vec2_s32_t *dst, const vec2_s32_t *src);
void vec2_s32_dot(vec2_s32_t *dst, const vec2_s32_t *a, const vec2_s32_t *b);

void vec3_s32_add(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b);
void vec3_s32_sub(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b);
void vec3_s32_mul(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b);
void vec3_s32_div(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b);
f32 vec3_s32_len(const vec3_s32_t *vec);
void vec3_s32_norm(vec3_s32_t *dst, const vec3_s32_t *src);
void vec3_s32_dot(vec3_s32_t *dst, const vec3_s32_t *a, const vec3_s32_t *b);

void vec4_s32_add(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b);
void vec4_s32_sub(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b);
void vec4_s32_mul(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b);
void vec4_s32_div(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b);
f32 vec4_s32_len(const vec4_s32_t *vec);
void vec4_s32_norm(vec4_s32_t *dst, const vec4_s32_t *src);
void vec4_s32_dot(vec4_s32_t *dst, const vec4_s32_t *a, const vec4_s32_t *b);

#endif
