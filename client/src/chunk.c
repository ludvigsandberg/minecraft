/*****************************************************************************
 * File:        chunk.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "client/chunk.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "common/pp.h"
#include "client/arr_f32.h"
#include "client/arr_u32.h"
#include "client/world.h"

static const f32 cube_positions[72] = {
    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, 0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
    0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f};

static const f32 cube_uvs[48] = {
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

static const int cube_face_dirs[6][3] = {
    {0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0},
};

static const f32 cube_shadows[6] = {0.90f, 0.60f, 0.50f, 0.70f, 1.00f, 0.55f};

static int is_face_visible(blocks_t blocks, int x, int y, int z, int dx,
                           int dy, int dz) {
    int nx;
    int ny;
    int nz;

    nx = x + dx;
    ny = y + dy;
    nz = z + dz;

    if (nx < 0 || nx >= CHUNK_SIZE || ny < 0 || ny >= CHUNK_SIZE || nz < 0 ||
        nz >= CHUNK_SIZE) {
        return TRUE;
    }

    return blocks[INDEX_3D(nx, ny, nz, CHUNK_SIZE)] == BLOCK_AIR;
}

static u8 get_face_light_level(chunk_t *chunk, int x, int y, int z, int dx,
                               int dy, int dz, const world_t *world) {
    int nx;
    int ny;
    int nz;
    s64 chunk_dx;
    s64 chunk_dy;
    s64 chunk_dz;
    s64 neighbor_chunk_x;
    s64 neighbor_chunk_y;
    s64 neighbor_chunk_z;
    s64 local_chunk_x;
    s64 local_chunk_y;
    s64 local_chunk_z;
    size_t index;
    chunk_t *neighbor;
    int lx;
    int ly;
    int lz;

    nx = x + dx;
    ny = y + dy;
    nz = z + dz;

    if (nx >= 0 && nx < CHUNK_SIZE && ny >= 0 && ny < CHUNK_SIZE && nz >= 0 &&
        nz < CHUNK_SIZE) {

        return chunk->light[INDEX_3D(nx, ny, nz, CHUNK_SIZE)];
    }

    chunk_dx = (nx < 0) ? -1 : (nx >= CHUNK_SIZE) ? 1 : 0;
    chunk_dy = (ny < 0) ? -1 : (ny >= CHUNK_SIZE) ? 1 : 0;
    chunk_dz = (nz < 0) ? -1 : (nz >= CHUNK_SIZE) ? 1 : 0;

    neighbor_chunk_x = chunk->coord.x + chunk_dx;
    neighbor_chunk_y = chunk->coord.y + chunk_dy;
    neighbor_chunk_z = chunk->coord.z + chunk_dz;

    local_chunk_x =
        neighbor_chunk_x - (world->center_chunk_coord.x - RENDER_DISTANCE);

    local_chunk_y =
        neighbor_chunk_y - (world->center_chunk_coord.y - RENDER_DISTANCE);

    local_chunk_z =
        neighbor_chunk_z - (world->center_chunk_coord.z - RENDER_DISTANCE);

    if (local_chunk_x < 0 || local_chunk_x >= LOADED_CHUNKS_LEN ||
        local_chunk_y < 0 || local_chunk_y >= LOADED_CHUNKS_LEN ||
        local_chunk_z < 0 || local_chunk_z >= LOADED_CHUNKS_LEN) {
        return 15;
    }

    index = (size_t)INDEX_3D(local_chunk_x, local_chunk_y, local_chunk_z,
                             LOADED_CHUNKS_LEN);

    neighbor = world->loaded_chunks[index];
    if (!neighbor)
        return 0;

    lx = (nx + CHUNK_SIZE) % CHUNK_SIZE;
    ly = (ny + CHUNK_SIZE) % CHUNK_SIZE;
    lz = (nz + CHUNK_SIZE) % CHUNK_SIZE;

    return neighbor->light[INDEX_3D(lx, ly, lz, CHUNK_SIZE)];
}

static void generate_mesh(chunk_t *chunk, const world_t *world) {
    arr_f32_t mesh_vertices;
    arr_u32_t mesh_indices;
    int x;
    int y;
    int z;

    /* Batch block faces into mesh. */

    arr_f32_new_reserve(&mesh_vertices, CHUNK_TOTAL * 6 * 4 * 7);
    arr_u32_new_reserve(&mesh_indices, CHUNK_TOTAL * 6 * 6);

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                u8 block;
                vec2_t uv_offs = {{0.0f, 0.0f}};
                int face_idx;

                block = chunk->blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];

                if (block == BLOCK_AIR) {
                    continue;
                }

                switch (block) {
                    case BLOCK_STONE:
                        VEC_U(uv_offs) = 16.0f / 256.0f;
                }

                /* For each face. */

                for (face_idx = 0; face_idx < 6; face_idx++) {
                    int dir[3];

                    dir[0] = cube_face_dirs[face_idx][0];
                    dir[1] = cube_face_dirs[face_idx][1];
                    dir[2] = cube_face_dirs[face_idx][2];

                    if (is_face_visible(chunk->blocks, x, y, z, dir[0], dir[1],
                                        dir[2])) {
                        u32 face_indices[6] = {0, 1, 2, 2, 3, 0};
                        int i;
                        u8 light;

                        /* Append indices. */

                        for (i = 0; i < 6; i++) {
                            size_t num_vertices;
                            u32 index;

                            num_vertices = mesh_vertices.size / 7;

                            index = (u32)num_vertices + face_indices[i];

                            arr_u32_append(&mesh_indices, &index);
                        }

                        /* Append vertices. */

                        light = get_face_light_level(chunk, x, y, z, dir[0],
                                                     dir[1], dir[2], world);

                        for (i = 0; i < 4; i++) {
                            size_t p;
                            size_t t;
                            f32 vertex[7];

                            p = (size_t)face_idx * 12 + (size_t)i * 3;
                            t = (size_t)face_idx * 8 + (size_t)i * 2;

                            vertex[0] = cube_positions[p + 0] + (f32)x;
                            vertex[1] = cube_positions[p + 1] + (f32)y;
                            vertex[2] = cube_positions[p + 2] + (f32)z;

                            vertex[3] =
                                cube_uvs[t + 0] / 16.0f + VEC_U(uv_offs);
                            vertex[4] =
                                cube_uvs[t + 1] / 16.0f + VEC_V(uv_offs);

                            vertex[5] = cube_shadows[face_idx];

                            vertex[6] = (f32)light;

                            arr_f32_append_n(&mesh_vertices, 7, vertex);
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
                 (GLsizeiptr)(mesh_vertices.size * sizeof(f32)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_vertices.size * sizeof(f32)),
                    mesh_vertices.data);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh_indices.size * sizeof(u32)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_indices.size * sizeof(u32)),
                    mesh_indices.data);

    chunk->num_indices = mesh_indices.size;

    /* Cleanup. */

    arr_u32_free(&mesh_indices);
    arr_f32_free(&mesh_vertices);
}

void chunk_new(chunk_t *chunk, blocks_t blocks, const coord_t *chunk_coord,
               world_t *world) {
    (void)world;

    /* Init. */

    chunk->dirty = true;

    memcpy(chunk->blocks, blocks, CHUNK_TOTAL * sizeof(u8));
    chunk->coord = *chunk_coord;

    /* Create buffers. */

    glGenVertexArrays(1, &chunk->vertex_array);
    glBindVertexArray(chunk->vertex_array);

    glGenBuffers(1, &chunk->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_TOTAL * 6 * 4 * 7 * sizeof(f32), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(f32),
                          (void *)0); /* Position. */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(f32),
                          (void *)(3 * sizeof(f32))); /* UV. */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(f32),
                          (void *)(5 * sizeof(f32))); /* Shadow. */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(f32),
                          (void *)(6 * sizeof(f32))); /* Light. */
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &chunk->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, CHUNK_TOTAL * 6 * 6 * sizeof(u32),
                 NULL, GL_DYNAMIC_DRAW); /* Pre allocate. */
}

void chunk_free(chunk_t *chunk) {
    glDeleteBuffers(1, &chunk->element_buffer);
    glDeleteBuffers(1, &chunk->vertex_buffer);
    glDeleteVertexArrays(1, &chunk->vertex_array);
}

void chunk_update(chunk_t *chunk, const world_t *world) {
    if (chunk->dirty) {
        chunk_calculate_light(chunk, world);
        generate_mesh(chunk, world);

        chunk->dirty = false;
    }
}
