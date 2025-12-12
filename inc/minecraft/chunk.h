#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stddef.h>

#include <glad/glad.h>
#include <meta.h>

#define BLOCK_AIR   0
#define BLOCK_GRASS 1
#define BLOCK_STONE 2

#define CHUNK_SIZE  16
#define CHUNK_TOTAL CHUNK_SIZE *CHUNK_SIZE *CHUNK_SIZE
typedef uint8_t blocks_t[CHUNK_TOTAL];

typedef struct {
    blocks_t blocks;

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
    size_t num_indices;
} chunk_t;

void chunk_new(chunk_t *chunk, blocks_t blocks);
void chunk_free(chunk_t chunk);

#endif
