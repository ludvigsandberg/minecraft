#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>

#include <glad/glad.h>

#include "coord.h"

#define BLOCK_AIR     0
#define BLOCK_GRASS   1
#define BLOCK_STONE   2
#define BLOCK_BEDROCK 3

#define CHUNK_SIZE  16
#define CHUNK_TOTAL CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

typedef struct {
    chunk_coord_t coord;
} chunk_job_t;

typedef struct {
    chunk_coord_t coord;
    unsigned char blocks[CHUNK_TOTAL];
} chunk_result_t;

typedef struct {
    chunk_coord_t coord;

    unsigned char blocks[CHUNK_TOTAL];

    /* First 4 bits = skylight, last 4 bits = block light. */
    unsigned char light[CHUNK_TOTAL];

    int dirty; /* True if mesh needs to be regenerated. */

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
    size_t index_count;
} chunk_t;

struct world_s; /* Forward declare. */

void chunk_init(chunk_t *chunk, const unsigned char *blocks,
                const chunk_coord_t *coord);
void chunk_free(chunk_t *chunk);
void chunk_update(chunk_t *chunk, const struct world_s *world);

#endif
