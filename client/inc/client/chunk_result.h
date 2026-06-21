/*****************************************************************************
 * File:        chunk_result.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_CHUNK_RESULT_H
#define CLIENT_CHUNK_RESULT_H

#include "common/vec_s64.h"
#include "client/chunk.h"

typedef struct {
    vec3_s64_t coord;
    blocks_t blocks;
} chunk_result_t;

#endif
