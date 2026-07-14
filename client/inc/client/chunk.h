/*****************************************************************************
 * File:        chunk.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_CHUNK_H
#define CLIENT_CHUNK_H

#include <stddef.h>

#include <glad/glad.h>

#include "common/types.h"
#include "common/coord.h"

#define BLOCK_AIR   0
#define BLOCK_GRASS 1
#define BLOCK_STONE 2

#define CHUNK_SIZE  16
#define CHUNK_TOTAL CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

struct world_s; /* Forward declare. */

typedef u8 blocks_t[CHUNK_TOTAL];

typedef struct {
    coord_t coord; /* Chunk coord, not world coord. */

    blocks_t blocks;

    /* First 4 bits = skylight, last 4 bits = block light. */
    u8 light[CHUNK_TOTAL];

    int dirty; /* Whether mesh needs to be regenerated. */

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
    size_t num_indices;
} chunk_t;

void chunk_new(chunk_t *chunk, blocks_t blocks, const coord_t *chunk_coord,
               struct world_s *world);
void chunk_free(chunk_t *chunk);
void chunk_update(chunk_t *chunk, const struct world_s *world);

void chunk_calculate_light(chunk_t *chunk, const struct world_s *world);

uint8_t chunk_get_skylight(const chunk_t *chunk, const coord_t *block_pos);
void chunk_set_skylight(chunk_t *chunk, const coord_t *block_pos,
                        uint8_t intensity);
uint8_t chunk_get_block_light(const chunk_t *chunk, const coord_t *block_pos);
void chunk_set_block_light(chunk_t *chunk, const coord_t *block_pos,
                           uint8_t intensity);

#endif
