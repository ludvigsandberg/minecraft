/*****************************************************************************
 * File:        mat.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef COMMON_MAT_H
#define COMMON_MAT_H

#define MAT_DECL(TYPE, NAME)                                                  \
    typedef struct mat3x3_##NAME##_s {                                        \
        TYPE elems[9];                                                        \
    } mat3x3_##NAME##_t;                                                      \
                                                                              \
    typedef struct mat4x4_##NAME##_s {                                        \
        TYPE elems[16];                                                       \
    } mat4x4_##NAME##_t;

#endif
