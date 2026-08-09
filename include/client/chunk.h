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

struct chunk_job {
    struct coord coord;
};

struct chunk_result {
    struct coord coord;
    unsigned char blocks[CHUNK_TOTAL];
};

struct chunk {
    struct coord coord;

    unsigned char blocks[CHUNK_TOTAL];

    /* First 4 bits = skylight, last 4 bits = block light. */
    unsigned char light[CHUNK_TOTAL];

    /* TRUE if mesh needs to be regenerated. */
    int is_dirty;

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
    size_t index_count;
};

struct world;

void chunk_init(struct chunk *chunk, const unsigned char *blocks,
                const struct coord *coord);
void chunk_free(struct chunk *chunk);
void chunk_update(struct chunk *chunk, const struct world *world);

#endif
