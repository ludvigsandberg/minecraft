#include <chunk.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <linmath.h>
#include <meta.h>

static const float cube_positions[72] = {
    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, 0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
    0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f};

static const float cube_uvs[48] = {
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
    1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
    1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};

static const float cube_shadows[6] = {.5f, .75f, 1.f, .5f, .75f, 1.f};

static const unsigned int cube_indices[36] = {
    0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
    12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

void chunk_new(chunk_t *chunk, blocks_t blocks) {
    /* Construct chunk mesh. */

    arr(float) mesh_vertices;
    arr_new_reserve(mesh_vertices, CHUNK_TOTAL * (72 + 48 + 24));

    arr(uint32_t) mesh_indices;
    arr_new_reserve(mesh_indices, CHUNK_TOTAL * 36);

    size_t num_blocks = 0;

    for (size_t x = 0; x < CHUNK_SIZE; x++) {
        for (size_t y = 0; y < CHUNK_SIZE; y++) {
            for (size_t z = 0; z < CHUNK_SIZE; z++) {
                uint8_t block =
                    blocks[z * (CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + x];

                if (block == BLOCK_AIR) {
                    continue;
                }

                vec2 uv_offs = {0.f, 0.f};
                switch (block) {
                    case 2:
                        uv_offs[0] = 16.f / 256.f;
                }

                /* Append vertex position and uv to mesh. */
                for (size_t i = 0; i < 24; i++) {

                    float vertex[6] = {cube_positions[i * 3 + 0] + x,
                                       cube_positions[i * 3 + 1] + y,
                                       cube_positions[i * 3 + 2] + z,
                                       cube_uvs[i * 2 + 0] / 16.f + uv_offs[0],
                                       cube_uvs[i * 2 + 1] / 16.f + uv_offs[1],
                                       cube_shadows[i / 4]};

                    arr_append_n(mesh_vertices, 6, vertex);
                }

                /* Append vertex index to mesh indices. */
                for (size_t i = 0; i < 36; i++) {
                    size_t block_local_vertex_index =
                        num_blocks * 24 + cube_indices[i];
                    arr_append(mesh_indices, block_local_vertex_index);
                }

                num_blocks++;
            }
        }
    }

    /* Upload mesh to GPU. */

    glGenVertexArrays(1, &chunk->vertex_array);
    glBindVertexArray(chunk->vertex_array);

    glGenBuffers(1, &chunk->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, alen(mesh_vertices) * sizeof(float),
                 mesh_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &chunk->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 alen(mesh_indices) * sizeof(uint32_t), mesh_indices,
                 GL_STATIC_DRAW);

    chunk->num_indices = alen(mesh_indices);

    /* Cleanup. */

    arr_free(mesh_indices);
    arr_free(mesh_vertices);
}

void chunk_free(chunk_t chunk) {
    glDeleteBuffers(1, &chunk.element_buffer);
    glDeleteBuffers(1, &chunk.vertex_buffer);
    glDeleteVertexArrays(1, &chunk.vertex_array);
}
