#include <minecraft/chunk.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <linmath.h>
#include <meta.h>

static const float cube_positions[72] = {
    // front
    -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    // back
    0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f,
    // left
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
    -0.5f,
    // right
    0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
    // top
    -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
    // bottom
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
    0.5f};

static const float cube_uvs[48] = {
    // front
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f,
    // back
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f,
    // left
    1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
    // right
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f,
    // top
    0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f,
    // bottom
    0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};

static const float cube_shadows[6] = {
    // front
    0.90f,
    // back
    0.60f,
    // left
    0.50f,
    // right
    0.70f,
    // top
    1.00f,
    // bottom
    0.55f};

static bool is_face_visible(blocks_t blocks, int x, int y, int z, int dx,
                            int dy, int dz) {
    int nx = x + dx;
    int ny = y + dy;
    int nz = z + dz;

    if (nx < 0 || nx >= CHUNK_SIZE || ny < 0 || ny >= CHUNK_SIZE || nz < 0 ||
        nz >= CHUNK_SIZE) {
        return true;
    }

    return blocks[nz * (CHUNK_SIZE * CHUNK_SIZE) + ny * CHUNK_SIZE + nx] ==
           BLOCK_AIR;
}

void chunk_new(chunk_t *chunk, blocks_t blocks) {
    // batch blocks into mesh

    arr(float) mesh_vertices;
    arr_new_reserve(mesh_vertices, CHUNK_TOTAL * (72 + 48 + 24));

    arr(uint32_t) mesh_indices;
    arr_new_reserve(mesh_indices, CHUNK_TOTAL * 36);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t block =
                    blocks[z * (CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + x];

                if (block == BLOCK_AIR) {
                    continue;
                }

                vec2 uv_offs = {0.f, 0.f};
                switch (block) {
                    case BLOCK_STONE:
                        uv_offs[0] = 16.f / 256.f;
                }

                int face_dirs[6][3] = {
                    // front
                    {0, 0, 1},
                    // back
                    {0, 0, -1},
                    // left
                    {-1, 0, 0},
                    // right
                    {1, 0, 0},
                    // top
                    {0, 1, 0},
                    // bottom
                    {0, -1, 0},
                };

                // for each face

                for (int face_idx = 0; face_idx < 6; face_idx++) {
                    if (is_face_visible(
                            blocks, x, y, z, face_dirs[face_idx][0],
                            face_dirs[face_idx][1], face_dirs[face_idx][2])) {

                        // append indices

                        static const int face_indices[6] = {0, 1, 2, 2, 3, 0};

                        for (int i = 0; i < 6; i++) {
                            size_t vertex_size = 6;
                            size_t num_vertices =
                                alen(mesh_vertices) / vertex_size;

                            unsigned int index =
                                num_vertices + face_indices[i];

                            arr_append(mesh_indices, index);
                        }

                        // append vertices
                        for (int v = 0; v < 4; v++) {
                            size_t p = face_idx * 12 + v * 3;
                            size_t t = face_idx * 8 + v * 2;

                            float vertex[6] = {
                                cube_positions[p + 0] + x,
                                cube_positions[p + 1] + y,
                                cube_positions[p + 2] + z,

                                cube_uvs[t + 0] / 16.f + uv_offs[0],
                                cube_uvs[t + 1] / 16.f + uv_offs[1],

                                cube_shadows[face_idx]};

                            arr_append_n(mesh_vertices, 6, vertex);
                        }
                    }
                }
            }
        }
    }

    // upload mesh to gpu

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

    // cleanup

    arr_free(mesh_indices);
    arr_free(mesh_vertices);
}

void chunk_free(chunk_t chunk) {
    glDeleteBuffers(1, &chunk.element_buffer);
    glDeleteBuffers(1, &chunk.vertex_buffer);
    glDeleteVertexArrays(1, &chunk.vertex_array);
}
