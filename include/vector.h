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

typedef struct vec2_s {
    float elems[2];
} vec2_t;

typedef struct vec3_s {
    float elems[3];
} vec3_t;

typedef struct vec4_s {
    float elems[4];
} vec4_t;

vec2_t vec2_add(const vec2_t *a, const vec2_t *b);
vec2_t vec2_sub(const vec2_t *a, const vec2_t *b);
vec2_t vec2_mul(const vec2_t *a, const vec2_t *b);
vec2_t vec2_div(const vec2_t *a, const vec2_t *b);
float vec2_len(const vec2_t *vec);
vec2_t vec2_norm(const vec2_t *vec);
float vec2_dot(const vec2_t *a, const vec2_t *b);

vec3_t vec3_add(const vec3_t *a, const vec3_t *b);
vec3_t vec3_sub(const vec3_t *a, const vec3_t *b);
vec3_t vec3_mul(const vec3_t *a, const vec3_t *b);
vec3_t vec3_div(const vec3_t *a, const vec3_t *b);
float vec3_len(const vec3_t *vec);
vec3_t vec3_norm(const vec3_t *vec);
float vec3_dot(const vec3_t *a, const vec3_t *b);
vec3_t vec3_cross(const vec3_t *a, const vec3_t *b);

vec4_t vec4_add(const vec4_t *a, const vec4_t *b);
vec4_t vec4_sub(const vec4_t *a, const vec4_t *b);
vec4_t vec4_mul(const vec4_t *a, const vec4_t *b);
vec4_t vec4_div(const vec4_t *a, const vec4_t *b);
float vec4_len(const vec4_t *vec);
vec4_t vec4_norm(const vec4_t *vec);
float vec4_dot(const vec4_t *a, const vec4_t *b);

#endif
