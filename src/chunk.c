#include <darkcraft/chunk.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <linmath.h>
#include <meta.h>

#include <darkcraft/world.h>

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

static const int cube_face_dirs[6][3] = {
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

static uint8_t get_face_light_level(chunk_t *chunk, int x, int y, int z,
                                    int dx, int dy, int dz,
                                    const world_t *world) {
    int nx = x + dx;
    int ny = y + dy;
    int nz = z + dz;

    if (nx >= 0 && nx < CHUNK_SIZE && ny >= 0 && ny < CHUNK_SIZE && nz >= 0 &&
        nz < CHUNK_SIZE) {

        return chunk
            ->light[nz * (CHUNK_SIZE * CHUNK_SIZE) + ny * CHUNK_SIZE + nx];
    }

    int chunk_dx = (nx < 0) ? -1 : (nx >= CHUNK_SIZE) ? 1 : 0;
    int chunk_dy = (ny < 0) ? -1 : (ny >= CHUNK_SIZE) ? 1 : 0;
    int chunk_dz = (nz < 0) ? -1 : (nz >= CHUNK_SIZE) ? 1 : 0;

    int neighbor_chunk_x = chunk->coord[0] + chunk_dx;
    int neighbor_chunk_y = chunk->coord[1] + chunk_dy;
    int neighbor_chunk_z = chunk->coord[2] + chunk_dz;

    int local_chunk_x =
        neighbor_chunk_x - (world->center_chunk_coord[0] - RENDER_DISTANCE);

    int local_chunk_y =
        neighbor_chunk_y - (world->center_chunk_coord[1] - RENDER_DISTANCE);

    int local_chunk_z =
        neighbor_chunk_z - (world->center_chunk_coord[2] - RENDER_DISTANCE);

    if (local_chunk_x < 0 || local_chunk_x >= LOADED_CHUNKS_LEN ||
        local_chunk_y < 0 || local_chunk_y >= LOADED_CHUNKS_LEN ||
        local_chunk_z < 0 || local_chunk_z >= LOADED_CHUNKS_LEN) {
        return 15;
    }

    size_t index = local_chunk_z * (LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN) +
                   local_chunk_y * LOADED_CHUNKS_LEN + local_chunk_x;

    chunk_t *neighbor = world->loaded_chunks[index];
    if (!neighbor)
        return 15;

    int lx = (nx + CHUNK_SIZE) % CHUNK_SIZE;
    int ly = (ny + CHUNK_SIZE) % CHUNK_SIZE;
    int lz = (nz + CHUNK_SIZE) % CHUNK_SIZE;

    return neighbor
        ->light[lz * (CHUNK_SIZE * CHUNK_SIZE) + ly * CHUNK_SIZE + lx];
}

static void generate_mesh(chunk_t *chunk, const world_t *world) {
    // batch blocks into mesh

    arr(float) mesh_vertices;
    arr_new_reserve(mesh_vertices, CHUNK_TOTAL * 6 * 4 * 7);

    arr(uint32_t) mesh_indices;
    arr_new_reserve(mesh_indices, CHUNK_TOTAL * 6 * 6);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                size_t block_idx =
                    z * (CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + x;

                uint8_t block = chunk->blocks[block_idx];

                if (block == BLOCK_AIR) {
                    continue;
                }

                vec2 uv_offs = {0.f, 0.f};
                switch (block) {
                    case BLOCK_STONE:
                        uv_offs[0] = 16.f / 256.f;
                }

                // for each face

                for (int face_idx = 0; face_idx < 6; face_idx++) {
                    const int dir[3] = {cube_face_dirs[face_idx][0],
                                        cube_face_dirs[face_idx][1],
                                        cube_face_dirs[face_idx][2]};

                    if (is_face_visible(chunk->blocks, x, y, z, dir[0], dir[1],
                                        dir[2])) {

                        // append indices

                        static const int face_indices[6] = {0, 1, 2, 2, 3, 0};

                        for (int i = 0; i < 6; i++) {
                            size_t num_vertices = alen(mesh_vertices) / 7;

                            unsigned int index =
                                num_vertices + face_indices[i];

                            arr_append(mesh_indices, index);
                        }

                        // append vertices

                        uint8_t light = get_face_light_level(
                            chunk, x, y, z, dir[0], dir[1], dir[2], world);

                        for (int v = 0; v < 4; v++) {
                            size_t p = face_idx * 12 + v * 3;
                            size_t t = face_idx * 8 + v * 2;

                            float vertex[7] = {
                                cube_positions[p + 0] + x,
                                cube_positions[p + 1] + y,
                                cube_positions[p + 2] + z,

                                cube_uvs[t + 0] / 16.f + uv_offs[0],
                                cube_uvs[t + 1] / 16.f + uv_offs[1],

                                cube_shadows[face_idx],

                                (float)light};

                            arr_append_n(mesh_vertices, 7, vertex);
                        }
                    }
                }
            }
        }
    }

    // upload mesh to gpu

    glBindVertexArray(chunk->vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, alen(mesh_vertices) * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW); // buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, alen(mesh_vertices) * sizeof(float),
                    mesh_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 alen(mesh_indices) * sizeof(uint32_t), NULL,
                 GL_DYNAMIC_DRAW); // buffer orphaning
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    alen(mesh_indices) * sizeof(uint32_t), mesh_indices);

    chunk->num_indices = alen(mesh_indices);

    // cleanup

    arr_free(mesh_indices);
    arr_free(mesh_vertices);
}

static void calculate_light(chunk_t *chunk, const world_t *world) {
    struct next_block_s {
        coord_t chunk;
        coord_t local;
    };

    queue(struct next_block_s) bfs_queue;
    queue_new(bfs_queue, 4096 * 4);

    // lighting pass 1, skylight

    // for each column
    for (int64_t x = 0; x < CHUNK_SIZE; x++) {
        for (int64_t z = 0; z < CHUNK_SIZE; z++) {
            bool column_has_sunlight = false;
            bool is_topmost_chunk    = true;

            // check if chunk above lets through skylight for this column
            for (int64_t chunk_y = chunk->coord[1] + 1;
                 chunk_y <= world->center_chunk_coord[1] + RENDER_DISTANCE;
                 chunk_y++) {
                coord_t world_chunk_coord = {chunk->coord[0], chunk_y,
                                             chunk->coord[2]};

                const chunk_t *skylight_chunk =
                    world->loaded_chunks[chunk_coord_to_index(
                        world_chunk_coord, world->center_chunk_coord)];

                // skip if not loaded
                if (!skylight_chunk) {
                    continue;
                } else {
                    is_topmost_chunk = false;
                }

                size_t i = z * (CHUNK_SIZE * CHUNK_SIZE) + 0 + x;

                // check if bottom block is transparent and has skylight
                if (skylight_chunk->blocks[i] == BLOCK_AIR &&
                    skylight_chunk->light[i] == 15) {
                    column_has_sunlight = true;
                    break;
                }
            }

            if (is_topmost_chunk) {
                column_has_sunlight = true;
            } else if (!column_has_sunlight) {
                continue;
            }

            // propagate skylight down until opaque block
            // invalidate chunks beneath in the process

            for (int64_t chunk_y = chunk->coord[1];
                 chunk_y >= world->center_chunk_coord[1] - RENDER_DISTANCE;
                 chunk_y--) {
                coord_t world_chunk_coord = {chunk->coord[0], chunk_y,
                                             chunk->coord[2]};

                chunk_t *skylight_chunk =
                    world->loaded_chunks[chunk_coord_to_index(
                        world_chunk_coord, world->center_chunk_coord)];

                // skip if not loaded
                if (!skylight_chunk) {
                    continue;
                }

                for (int64_t y = 15; y >= 0; y--) {
                    size_t i =
                        z * (CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + x;

                    if (skylight_chunk->blocks[i] != BLOCK_AIR) {
                        goto propagate_skylight_end;
                    }

                    if (skylight_chunk->light[i] != 15) {
                        skylight_chunk->light[i] = 15;

                        if (skylight_chunk != chunk) {
                            skylight_chunk->dirty = true;
                        }

                        // push new skylight to floodfill queue

                        struct next_block_s next;
                        next.chunk[0] = skylight_chunk->coord[0];
                        next.chunk[1] = skylight_chunk->coord[1];
                        next.chunk[2] = skylight_chunk->coord[2];
                        next.local[0] = x;
                        next.local[1] = y;
                        next.local[2] = z;

                        queue_push(bfs_queue, next);
                    }
                }
            }
        propagate_skylight_end:;
        }
    }

    // lighting pass 2, bfs floodfill

    while (!queue_empty(bfs_queue)) {
        struct next_block_s current = queue_pop(bfs_queue);
    }

    queue_free(bfs_queue);
}

void chunk_new(chunk_t *chunk, blocks_t blocks, coord_t chunk_coord) {
    chunk->dirty = true;

    memcpy(chunk->blocks, blocks, CHUNK_TOTAL * sizeof(uint8_t));
    memcpy(chunk->coord, chunk_coord, sizeof(coord_t));

    memset(chunk->light, 0, CHUNK_TOTAL * sizeof(uint8_t));

    // create buffers

    glGenVertexArrays(1, &chunk->vertex_array);
    glBindVertexArray(chunk->vertex_array);

    glGenBuffers(1, &chunk->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_TOTAL * 6 * 4 * 7 * sizeof(float),
                 NULL, GL_DYNAMIC_DRAW); // pre allocate
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)0); // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(3 * sizeof(float))); // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(5 * sizeof(float))); // shadow
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(6 * sizeof(float))); // light
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &chunk->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 CHUNK_TOTAL * 6 * 6 * sizeof(uint32_t), NULL,
                 GL_DYNAMIC_DRAW); // pre allocate
}

void chunk_free(chunk_t chunk) {
    glDeleteBuffers(1, &chunk.element_buffer);
    glDeleteBuffers(1, &chunk.vertex_buffer);
    glDeleteVertexArrays(1, &chunk.vertex_array);
}

void chunk_update(chunk_t *chunk, world_t *world) {
    if (chunk->dirty) {
        calculate_light(chunk, world);
        generate_mesh(chunk, world);

        chunk->dirty = false;
    }
}
