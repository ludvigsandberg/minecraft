#include "client/chunk.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "macros.h"
#include "client/world.h"

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
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

static const int cube_face_dirs[6][3] = {
    {0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0},
};

static const float cube_shadows[6] = {0.90f, 0.60f, 0.50f,
                                      0.70f, 1.00f, 0.55f};

static int is_face_visible(unsigned char *blocks, int x, int y, int z,
                           int dir_x, int dir_y, int dir_z) {
    int neighbor_x;
    int neighbor_y;
    int neighbor_z;

    neighbor_x = x + dir_x;
    neighbor_y = y + dir_y;
    neighbor_z = z + dir_z;

    if (neighbor_x < 0 || neighbor_x >= CHUNK_SIZE || neighbor_y < 0 ||
        neighbor_y >= CHUNK_SIZE || neighbor_z < 0 ||
        neighbor_z >= CHUNK_SIZE) {
        return TRUE;
    }

    return blocks[INDEX_3D(neighbor_x, neighbor_y, neighbor_z, CHUNK_SIZE)] ==
           BLOCK_AIR;
}

static unsigned char get_face_light_level(chunk_t *chunk, int x, int y, int z,
                                          int dir_x, int dir_y, int dir_z,
                                          const world_t *world) {
    (void)chunk;
    (void)x;
    (void)y;
    (void)z;
    (void)dir_x;
    (void)dir_y;
    (void)dir_z;
    (void)world;

    return 16;
}

static void generate_mesh(chunk_t *chunk, const world_t *world) {
    float mesh_vertices[CHUNK_TOTAL * 6 * 4 * 7];
    size_t mesh_vertex_count = 0;

    unsigned int mesh_indices[CHUNK_TOTAL * 6 * 6];
    size_t mesh_index_count = 0;

    int x;
    int y;
    int z;

    /* Batch block faces into mesh. */

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                unsigned char block;
                struct vector2 uv_offset;
                int face_idx;

                block = chunk->blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];

                if (block == BLOCK_AIR) {
                    continue;
                }

                uv_offset.VEC_U = 0.0f;
                uv_offset.VEC_V = 0.0f;

                switch (block) {
                    case BLOCK_STONE:
                        uv_offset.VEC_U = 16.0f / 256.0f;
                        break;

                    case BLOCK_BEDROCK:
                        uv_offset.VEC_U = 16.0f / 256.0f;
                        uv_offset.VEC_V = 16.0f / 256.0f;
                        break;
                }

                /* For each face. */

                for (face_idx = 0; face_idx < 6; face_idx++) {
                    int dir_x;
                    int dir_y;
                    int dir_z;

                    dir_x = cube_face_dirs[face_idx][0];
                    dir_y = cube_face_dirs[face_idx][1];
                    dir_z = cube_face_dirs[face_idx][2];

                    if (is_face_visible(chunk->blocks, x, y, z, dir_x, dir_y,
                                        dir_z)) {
                        int i;
                        unsigned int face_indices[6] = {0, 1, 2, 2, 3, 0};
                        unsigned char light;

                        /* Append indices. */

                        for (i = 0; i < 6; i++) {
                            size_t num_vertices;
                            unsigned int index;

                            num_vertices = mesh_vertex_count / 7;

                            index =
                                (unsigned int)num_vertices + face_indices[i];

                            mesh_indices[mesh_index_count++] = index;
                        }

                        /* Append vertices. */

                        light = get_face_light_level(chunk, x, y, z, dir_x,
                                                     dir_y, dir_z, world);

                        for (i = 0; i < 4; i++) {
                            size_t p;
                            size_t t;
                            float *vertex = mesh_vertices + mesh_vertex_count;

                            p = (size_t)face_idx * 12 + (size_t)i * 3;
                            t = (size_t)face_idx * 8 + (size_t)i * 2;

                            vertex[0] = (float)(chunk->coord.x * CHUNK_SIZE) +
                                        cube_positions[p + 0] + (float)x;
                            vertex[1] = (float)(chunk->coord.y * CHUNK_SIZE) +
                                        cube_positions[p + 1] + (float)y;
                            vertex[2] = (float)(chunk->coord.z * CHUNK_SIZE) +
                                        cube_positions[p + 2] + (float)z;
                            vertex[3] =
                                cube_uvs[t + 0] / 16.0f + uv_offset.VEC_U;
                            vertex[4] =
                                cube_uvs[t + 1] / 16.0f + uv_offset.VEC_V;
                            vertex[5] = cube_shadows[face_idx];
                            vertex[6] = (float)light;

                            mesh_vertex_count += 7;
                        }
                    }
                }
            }
        }
    }

    /* Upload mesh to GPU. */

    glBindVertexArray(chunk->vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh_vertex_count * sizeof(float)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_vertex_count * sizeof(float)),
                    mesh_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh_index_count * sizeof(unsigned int)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_index_count * sizeof(unsigned int)),
                    mesh_indices);

    chunk->index_count = mesh_index_count;
}

void chunk_init(chunk_t *chunk, const unsigned char *blocks,
                const chunk_coord_t *coord) {
    assert(chunk);
    assert(blocks);
    assert(coord);

    /* Init. */

    chunk->dirty = TRUE;

    memcpy(chunk->blocks, blocks, CHUNK_TOTAL * sizeof(*blocks));
    chunk->coord = *coord;

    /* Create buffers. */

    glGenVertexArrays(1, &chunk->vertex_array);
    glBindVertexArray(chunk->vertex_array);

    glGenBuffers(1, &chunk->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_TOTAL * 6 * 4 * 7 * sizeof(float),
                 NULL, GL_DYNAMIC_DRAW); /* Pre allocate. */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)0); /* Position. */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(3 * sizeof(float))); /* UV. */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(5 * sizeof(float))); /* Shadow. */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(6 * sizeof(float))); /* Light. */
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &chunk->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 CHUNK_TOTAL * 6 * 6 * sizeof(unsigned int), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */
}

void chunk_free(chunk_t *chunk) {
    glDeleteBuffers(1, &chunk->element_buffer);
    glDeleteBuffers(1, &chunk->vertex_buffer);
    glDeleteVertexArrays(1, &chunk->vertex_array);
}

void chunk_update(chunk_t *chunk, const world_t *world) {
    if (chunk->dirty) {
        generate_mesh(chunk, world);

        chunk->dirty = FALSE;
    }
}
