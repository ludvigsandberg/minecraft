/*****************************************************************************
 * File:        vec.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_VEC_H
#define COMMON_VEC_H

#include <assert.h>

#define VEC_DECL(TYPE, NAME)                                                  \
    typedef union vec2_##NAME##_u {                                           \
        struct {                                                              \
            TYPE x;                                                           \
            TYPE y;                                                           \
        } pos;                                                                \
        struct {                                                              \
            TYPE u;                                                           \
            TYPE v;                                                           \
        } uv;                                                                 \
        TYPE elems[2];                                                        \
    } vec2_##NAME##_t;                                                        \
                                                                              \
    typedef union vec3_##NAME##_u {                                           \
        struct {                                                              \
            TYPE x;                                                           \
            TYPE y;                                                           \
            TYPE z;                                                           \
        } pos;                                                                \
        struct {                                                              \
            TYPE r;                                                           \
            TYPE g;                                                           \
            TYPE b;                                                           \
        } col;                                                                \
        struct {                                                              \
            TYPE u;                                                           \
            TYPE v;                                                           \
            TYPE w;                                                           \
        } uv;                                                                 \
        TYPE elems[3];                                                        \
    } vec3_##NAME##_t;                                                        \
                                                                              \
    typedef union vec4_##NAME##_u {                                           \
        struct {                                                              \
            TYPE x;                                                           \
            TYPE y;                                                           \
            TYPE z;                                                           \
            TYPE w;                                                           \
        } pos;                                                                \
        struct {                                                              \
            TYPE r;                                                           \
            TYPE g;                                                           \
            TYPE b;                                                           \
            TYPE a;                                                           \
        } col;                                                                \
        struct {                                                              \
            TYPE u;                                                           \
            TYPE v;                                                           \
            TYPE w;                                                           \
            TYPE q;                                                           \
        } uv;                                                                 \
        TYPE elems[4];                                                        \
    } vec4_##NAME##_t;                                                        \
                                                                              \
    void vec2_##NAME##_add(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b);                         \
    void vec2_##NAME##_sub(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b);                         \
    void vec2_##NAME##_mul(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b);                         \
    void vec2_##NAME##_div(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b);                         \
    TYPE vec2_##NAME##_len(const vec2_##NAME##_t *vec);                       \
    void vec2_##NAME##_norm(vec2_##NAME##_t *dst,                             \
                            const vec2_##NAME##_t *src);                      \
    void vec2_##NAME##_dot(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b);                         \
                                                                              \
    void vec3_##NAME##_add(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b);                         \
    void vec3_##NAME##_sub(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b);                         \
    void vec3_##NAME##_mul(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b);                         \
    void vec3_##NAME##_div(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b);                         \
    TYPE vec3_##NAME##_len(const vec3_##NAME##_t *vec);                       \
    void vec3_##NAME##_norm(vec3_##NAME##_t *dst,                             \
                            const vec3_##NAME##_t *src);                      \
    void vec3_##NAME##_dot(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b);                         \
                                                                              \
    void vec4_##NAME##_add(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b);                         \
    void vec4_##NAME##_sub(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b);                         \
    void vec4_##NAME##_mul(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b);                         \
    void vec4_##NAME##_div(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b);                         \
    TYPE vec4_##NAME##_len(const vec4_##NAME##_t *vec);                       \
    void vec4_##NAME##_norm(vec4_##NAME##_t *dst,                             \
                            const vec4_##NAME##_t *src);                      \
    void vec4_##NAME##_dot(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b);

#define VEC_DEF(TYPE, NAME, SQRT_FN)                                          \
    void vec2_##NAME##_add(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x + b->pos.x;                                     \
        dst->pos.y = a->pos.y + b->pos.y;                                     \
    }                                                                         \
                                                                              \
    void vec2_##NAME##_sub(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x - b->pos.x;                                     \
        dst->pos.y = a->pos.y - b->pos.y;                                     \
    }                                                                         \
                                                                              \
    void vec2_##NAME##_mul(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x * b->pos.x;                                     \
        dst->pos.y = a->pos.y * b->pos.y;                                     \
    }                                                                         \
                                                                              \
    void vec2_##NAME##_div(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x / b->pos.x;                                     \
        dst->pos.y = a->pos.y / b->pos.y;                                     \
    }                                                                         \
                                                                              \
    TYPE vec2_##NAME##_len(const vec2_##NAME##_t *vec) {                      \
        assert(vec);                                                          \
        return SQRT_FN(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y);    \
    }                                                                         \
                                                                              \
    void vec2_##NAME##_norm(vec2_##NAME##_t *dst,                             \
                            const vec2_##NAME##_t *src) {                     \
        TYPE len;                                                             \
        assert(dst);                                                          \
        assert(src);                                                          \
        len        = vec2_##NAME##_len(src);                                  \
        dst->pos.x = (TYPE)(src->pos.x / len);                                \
        dst->pos.y = (TYPE)(src->pos.y / len);                                \
    }                                                                         \
                                                                              \
    void vec2_##NAME##_dot(vec2_##NAME##_t *dst, const vec2_##NAME##_t *a,    \
                           const vec2_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y;               \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_add(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x + b->pos.x;                                     \
        dst->pos.y = a->pos.y + b->pos.y;                                     \
        dst->pos.z = a->pos.z + b->pos.z;                                     \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_sub(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x - b->pos.x;                                     \
        dst->pos.y = a->pos.y - b->pos.y;                                     \
        dst->pos.z = a->pos.z - b->pos.z;                                     \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_mul(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x * b->pos.x;                                     \
        dst->pos.y = a->pos.y * b->pos.y;                                     \
        dst->pos.z = a->pos.z * b->pos.z;                                     \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_div(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x / b->pos.x;                                     \
        dst->pos.y = a->pos.y / b->pos.y;                                     \
        dst->pos.z = a->pos.z / b->pos.z;                                     \
    }                                                                         \
                                                                              \
    TYPE vec3_##NAME##_len(const vec3_##NAME##_t *vec) {                      \
        assert(vec);                                                          \
        return SQRT_FN(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y +    \
                       vec->pos.z * vec->pos.z);                              \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_norm(vec3_##NAME##_t *dst,                             \
                            const vec3_##NAME##_t *src) {                     \
        TYPE len;                                                             \
        assert(dst);                                                          \
        assert(src);                                                          \
        len        = vec3_##NAME##_len(src);                                  \
        dst->pos.x = (TYPE)(src->pos.x / len);                                \
        dst->pos.y = (TYPE)(src->pos.y / len);                                \
        dst->pos.z = (TYPE)(src->pos.z / len);                                \
    }                                                                         \
                                                                              \
    void vec3_##NAME##_dot(vec3_##NAME##_t *dst, const vec3_##NAME##_t *a,    \
                           const vec3_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x =                                                          \
            a->pos.x * b->pos.x + a->pos.y * b->pos.y + a->pos.z * b->pos.z;  \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_add(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x + b->pos.x;                                     \
        dst->pos.y = a->pos.y + b->pos.y;                                     \
        dst->pos.z = a->pos.z + b->pos.z;                                     \
        dst->pos.w = a->pos.w + b->pos.w;                                     \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_sub(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x - b->pos.x;                                     \
        dst->pos.y = a->pos.y - b->pos.y;                                     \
        dst->pos.z = a->pos.z - b->pos.z;                                     \
        dst->pos.w = a->pos.w - b->pos.w;                                     \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_mul(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x * b->pos.x;                                     \
        dst->pos.y = a->pos.y * b->pos.y;                                     \
        dst->pos.z = a->pos.z * b->pos.z;                                     \
        dst->pos.w = a->pos.w * b->pos.w;                                     \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_div(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x / b->pos.x;                                     \
        dst->pos.y = a->pos.y / b->pos.y;                                     \
        dst->pos.z = a->pos.z / b->pos.z;                                     \
        dst->pos.w = a->pos.w / b->pos.w;                                     \
    }                                                                         \
                                                                              \
    TYPE vec4_##NAME##_len(const vec4_##NAME##_t *vec) {                      \
        assert(vec);                                                          \
        return SQRT_FN(vec->pos.x * vec->pos.x + vec->pos.y * vec->pos.y +    \
                       vec->pos.z * vec->pos.z + vec->pos.w * vec->pos.w);    \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_norm(vec4_##NAME##_t *dst,                             \
                            const vec4_##NAME##_t *src) {                     \
        TYPE len;                                                             \
        assert(dst);                                                          \
        assert(src);                                                          \
        len        = vec4_##NAME##_len(src);                                  \
        dst->pos.x = (TYPE)(src->pos.x / len);                                \
        dst->pos.y = (TYPE)(src->pos.y / len);                                \
        dst->pos.z = (TYPE)(src->pos.z / len);                                \
        dst->pos.w = (TYPE)(src->pos.w / len);                                \
    }                                                                         \
                                                                              \
    void vec4_##NAME##_dot(vec4_##NAME##_t *dst, const vec4_##NAME##_t *a,    \
                           const vec4_##NAME##_t *b) {                        \
        assert(dst);                                                          \
        assert(a);                                                            \
        assert(b);                                                            \
        dst->pos.x = a->pos.x * b->pos.x + a->pos.y * b->pos.y +              \
                     a->pos.z * b->pos.z + a->pos.w * b->pos.w;               \
    }

#endif
