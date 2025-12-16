#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <meta.h>

#define BLOCK_AIR   0
#define BLOCK_GRASS 1
#define BLOCK_STONE 2

#define CHUNK_SIZE  16
#define CHUNK_TOTAL CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

// forward declare
struct world_t;

typedef int64_t coord_t[3];

typedef uint8_t blocks_t[CHUNK_TOTAL];

typedef struct {
    // chunk coord (not block coord)
    coord_t coord;

    blocks_t blocks;

    // light intensity 0-15
    uint8_t light[CHUNK_TOTAL];

    // mesh needs to be regenerated
    bool dirty;

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
    size_t num_indices;
} chunk_t;

void chunk_new(chunk_t *chunk, blocks_t blocks, coord_t chunk_coord);
void chunk_free(chunk_t chunk);
void chunk_update(chunk_t *chunk, const struct world_t *world);

#endif
