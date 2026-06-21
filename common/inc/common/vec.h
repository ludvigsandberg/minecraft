/*****************************************************************************
 * File:        vec.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_VEC_H
#define COMMON_VEC_H

#include "common/types.h"

typedef union vec2_u {
    struct {
        f32 x;
        f32 y;
    } pos;
    struct {
        f32 u;
        f32 v;
    } uv;
    f32 elems[2];
} vec2_t;

typedef union vec3_u {
    struct {
        f32 x;
        f32 y;
        f32 z;
    } pos;
    struct {
        f32 r;
        f32 g;
        f32 b;
    } color;
    struct {
        f32 u;
        f32 v;
        f32 w;
    } uv;
    f32 elems[3];
} vec3_t;

typedef union vec4_u {
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
    } color;
    struct {
        f32 u;
        f32 v;
        f32 w;
        f32 q;
    } uv;
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
