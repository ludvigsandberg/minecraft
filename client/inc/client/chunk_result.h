/*****************************************************************************
 * File:        chunk_result.h
 * Author:      ludvigsandberg
 * Date:        2026-06-21
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_CHUNK_RESULT_H
#define CLIENT_CHUNK_RESULT_H

#include "common/coord.h"
#include "client/chunk.h"

typedef struct {
    coord_t coord; /* Chunk coord, not world coord. */
    blocks_t blocks;
} chunk_result_t;

#endif
