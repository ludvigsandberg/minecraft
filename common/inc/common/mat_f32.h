/*****************************************************************************
 * File:        mat_f32.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef COMMON_MAT_F32_H
#define COMMON_MAT_F32_H

#include "common/types.h"

typedef union mat3x3_f32_u {
    f32 elems[9];
} mat3x3_f32_t;

typedef union mat4x4_f32_u {
    f32 elems[16];
} mat4x4_f32_t;

#endif
