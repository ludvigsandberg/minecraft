/*****************************************************************************
 * File:        world.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "client/world.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "common/mem.h"
#include "common/pp.h"
#include "client/chunk.h"
#include "client/gl.h"
#include "client/bitmap.h"

void generate(blocks_t blocks, const coord_t *chunk_coord) {
    s64 x;
    s64 y;
    s64 z;

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                u8 *block;
                coord_t world_coord;
                /*f32 noise_x;
                f32 noise_y;
                f32 noise_z;*/
                f32 n;

                block = &blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];

                world_coord.x = chunk_coord->x * CHUNK_SIZE + x;
                world_coord.y = chunk_coord->y * CHUNK_SIZE + y;
                world_coord.z = chunk_coord->z * CHUNK_SIZE + z;

                /*noise_x = (f32)(chunk_coord->x * CHUNK_SIZE + x) / 25.0f;
                noise_y = (f32)(chunk_coord->y * CHUNK_SIZE + y) / 25.0f;
                noise_z = (f32)(chunk_coord->z * CHUNK_SIZE + z) / 25.0f;*/

                n = (f32)((f64)rand() / RAND_MAX); /*stb_perlin_noise3(noise_x,
                                            noise_y, noise_z, 0, 0, 0);*/

                if (world_coord.y < -10) {
                    if (n > 0.2f) {
                        *block = BLOCK_AIR;
                    } else {
                        *block = BLOCK_STONE;
                    }
                } else {
                    if (n > 0.3f) {
                        *block = (n > 0.31f) ? BLOCK_STONE : BLOCK_GRASS;
                    } else {
                        *block = BLOCK_AIR;
                    }
                }
            }
        }
    }
}

int world_is_chunk_loaded(const world_t *world, const coord_t *chunk_coord,
                          chunk_t **chunk) {
    chunk_t *chunk_maybe_loaded;

    assert(world);
    assert(chunk_coord);
    assert(chunk);

    if (chunk_coord->x < world->center_chunk_coord.x - RENDER_DISTANCE ||
        chunk_coord->x > world->center_chunk_coord.x + RENDER_DISTANCE ||

        chunk_coord->y < world->center_chunk_coord.y - RENDER_DISTANCE ||
        chunk_coord->y > world->center_chunk_coord.y + RENDER_DISTANCE ||

        chunk_coord->z < world->center_chunk_coord.z - RENDER_DISTANCE ||
        chunk_coord->z > world->center_chunk_coord.z + RENDER_DISTANCE) {
        return false;
    }

    chunk_maybe_loaded = world->loaded_chunks[chunk_coord_to_index(
        chunk_coord, &world->center_chunk_coord)];

    if (!chunk_maybe_loaded) {
        return false;
    }

    if (chunk) {
        *chunk = chunk_maybe_loaded;
    }

    return true;
}

void world_to_local_chunk_coord(const coord_t *coord, const coord_t *center,
                                coord_t *out_local) {
    out_local->x = coord->x - center->x + RENDER_DISTANCE;
    out_local->y = coord->y - center->y + RENDER_DISTANCE;
    out_local->z = coord->z - center->z + RENDER_DISTANCE;
}

size_t local_chunk_coord_to_index(const coord_t *local) {
    return (size_t)INDEX_3D(local->x, local->y, local->z, LOADED_CHUNKS_LEN);
}

size_t chunk_coord_to_index(const coord_t *coord, const coord_t *center) {
    coord_t local;
    world_to_local_chunk_coord(coord, center, &local);

    return local_chunk_coord_to_index(&local);
}

static void *chunk_load_thread(void *ctx) {
    world_t *world = ctx;

    while (TRUE) {
        chunk_job_t *job = NULL;
        chunk_result_t *res;

        pthread_mutex_lock(&world->mutex);

        while (world->running && world->job_queue.size == 0) {
            pthread_cond_wait(&world->cond, &world->mutex);
        }

        if (!world->running) {
            pthread_mutex_unlock(&world->mutex);
            break;
        }

        if (world->job_queue.size > 0) {
            job = world->job_queue.data[world->job_queue.size - 1];

            world->job_queue.size--;
        }

        pthread_mutex_unlock(&world->mutex);

        res        = checked_malloc(sizeof(chunk_result_t));
        res->coord = job->coord;
        generate(res->blocks, &res->coord);

        free(job);

        pthread_mutex_lock(&world->mutex);
        arr_chunk_result_ptr_append(&world->result_queue, &res);
        pthread_mutex_unlock(&world->mutex);
    }

    return NULL;
}

static void load_chunk(world_t *world, const coord_t *chunk_coord) {
    chunk_job_t *job = checked_malloc(sizeof(chunk_job_t));
    job->coord       = *chunk_coord;

    pthread_mutex_lock(&world->mutex);
    arr_chunk_job_ptr_append(&world->job_queue, &job);
    pthread_cond_signal(&world->cond);
    pthread_mutex_unlock(&world->mutex);
}

void world_new(world_t *world) {
    coord_t chunk_coord;

    struct {
        u8 *data;
        int width;
        int height;
    } atlas;

    /* Load shaders. */

    world->shader_program = shader_program_new("client/res/chunk_vs.glsl",
                                               "client/res/chunk_fs.glsl");

    world->uniform_loc.texture =
        glGetUniformLocation(world->shader_program, "atlas");
    world->uniform_loc.model_matrix =
        glGetUniformLocation(world->shader_program, "model");
    world->uniform_loc.view_matrix =
        glGetUniformLocation(world->shader_program, "view");
    world->uniform_loc.projection_matrix =
        glGetUniformLocation(world->shader_program, "projection");
    world->uniform_loc.camera_pos =
        glGetUniformLocation(world->shader_program, "cameraPos");

    /* Load atlas. */

    bitmap_load(&atlas.data, &atlas.width, &atlas.height,
                "client/res/atlas.bmp");

    glGenTextures(1, &world->texture);
    glBindTexture(GL_TEXTURE_2D, world->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, atlas.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Setup chunk loading threads. */

    arr_chunk_job_ptr_new(&world->job_queue);
    arr_chunk_result_ptr_new(&world->result_queue);

    pthread_mutex_init(&world->mutex, NULL);
    pthread_cond_init(&world->cond, NULL);
    world->running = true;
    pthread_create(&world->thread, NULL, chunk_load_thread, world);

    /* Load chunks. */

    world->center_chunk_coord.x = 0;
    world->center_chunk_coord.y = 0;
    world->center_chunk_coord.z = 0;

    memset(world->loaded_chunks, 0, LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

    for (chunk_coord.x = -RENDER_DISTANCE; chunk_coord.x <= RENDER_DISTANCE;
         chunk_coord.x++) {
        for (chunk_coord.y = -RENDER_DISTANCE;
             chunk_coord.y <= RENDER_DISTANCE; chunk_coord.y++) {
            for (chunk_coord.z = -RENDER_DISTANCE;
                 chunk_coord.z <= RENDER_DISTANCE; chunk_coord.z++) {
                load_chunk(world, &chunk_coord);
            }
        }
    }
}

void world_update(world_t *world, const camera_t *cam) {
    coord_t camera_world_chunk_coord;
    size_t i;
    int camera_moved_to_different_chunk = FALSE;

    camera_world_chunk_coord.x = (s64)VEC_X(cam->pos) / CHUNK_SIZE;
    camera_world_chunk_coord.y = (s64)VEC_Y(cam->pos) / CHUNK_SIZE;
    camera_world_chunk_coord.z = (s64)VEC_Z(cam->pos) / CHUNK_SIZE;

    /* Poll threads for new chunks. */

    pthread_mutex_lock(&world->mutex);

    for (i = 0; i < world->result_queue.size; i++) {
        chunk_result_t *result;
        int chunk_within_region = TRUE;

        result = world->result_queue.data[i];

        if (result->coord.x < camera_world_chunk_coord.x - RENDER_DISTANCE ||
            result->coord.x > camera_world_chunk_coord.x + RENDER_DISTANCE ||

            result->coord.y < camera_world_chunk_coord.y - RENDER_DISTANCE ||
            result->coord.y > camera_world_chunk_coord.y + RENDER_DISTANCE ||

            result->coord.z < camera_world_chunk_coord.z - RENDER_DISTANCE ||
            result->coord.z > camera_world_chunk_coord.z + RENDER_DISTANCE) {
            break;
        }

        if (chunk_within_region) {
            chunk_t *chunk = checked_malloc(sizeof(chunk_t));
            chunk_new(chunk, result->blocks, &result->coord, world);

            world->loaded_chunks[chunk_coord_to_index(
                &result->coord, &world->center_chunk_coord)] = chunk;
        }

        free(result);
    }

    world->result_queue.size = 0;

    pthread_mutex_unlock(&world->mutex);

    /* Check if camera has moved to a different chunk. */
    if (world->center_chunk_coord.x != camera_world_chunk_coord.x ||
        world->center_chunk_coord.y != camera_world_chunk_coord.y ||
        world->center_chunk_coord.z != camera_world_chunk_coord.z) {
        camera_moved_to_different_chunk = TRUE;
    }

    /* Move and generate chunks. */
    if (camera_moved_to_different_chunk) {
        coord_t old_center_chunk_coord;
        coord_t chunk_coord_diff;
        chunk_t *old_loaded_chunks[LOADED_CHUNKS_TOTAL];
        s64 x;
        s64 y;
        s64 z;

        old_center_chunk_coord    = world->center_chunk_coord;
        world->center_chunk_coord = camera_world_chunk_coord;

        chunk_coord_diff.x =
            world->center_chunk_coord.x - old_center_chunk_coord.x;
        chunk_coord_diff.y =
            world->center_chunk_coord.y - old_center_chunk_coord.y;
        chunk_coord_diff.z =
            world->center_chunk_coord.z - old_center_chunk_coord.z;

        memcpy(old_loaded_chunks, world->loaded_chunks,
               LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

        /* For each chunk in the new region, check if we can copy the chunk
           from the previously loaded region or if a new one has to be
           generated. */

        for (x = 0; x < LOADED_CHUNKS_LEN; x++) {
            for (y = 0; y < LOADED_CHUNKS_LEN; y++) {
                for (z = 0; z < LOADED_CHUNKS_LEN; z++) {
                    coord_t local_chunk_coord;
                    coord_t old_local_chunk_coord;
                    int delete   = false;
                    int can_copy = true;

                    local_chunk_coord.x = x;
                    local_chunk_coord.y = y;
                    local_chunk_coord.z = z;

                    old_local_chunk_coord.x = x + chunk_coord_diff.x;
                    old_local_chunk_coord.y = y + chunk_coord_diff.y;
                    old_local_chunk_coord.z = z + chunk_coord_diff.z;

                    /* Free chunk if it has moved out of new area. */

                    if (local_chunk_coord.x < chunk_coord_diff.x ||
                        local_chunk_coord.x >=
                            chunk_coord_diff.x + LOADED_CHUNKS_LEN ||

                        local_chunk_coord.y < chunk_coord_diff.y ||
                        local_chunk_coord.y >=
                            chunk_coord_diff.y + LOADED_CHUNKS_LEN ||

                        local_chunk_coord.z < chunk_coord_diff.z ||
                        local_chunk_coord.z >=
                            chunk_coord_diff.z + LOADED_CHUNKS_LEN) {
                        delete = true;
                    }

                    if (delete) {
                        size_t idx =
                            local_chunk_coord_to_index(&local_chunk_coord);

                        if (old_loaded_chunks[idx]) {
                            coord_t below_chunk_coord;
                            chunk_t *below_chunk;

                            /* Set chunk below to dirty. */

                            below_chunk_coord = old_loaded_chunks[idx]->coord;
                            below_chunk_coord.y--;

                            if (world_is_chunk_loaded(
                                    world, &below_chunk_coord, &below_chunk)) {
                                below_chunk->dirty = true;
                            }

                            /* Free. */
                            chunk_free(old_loaded_chunks[idx]);
                            free(old_loaded_chunks[idx]);
                            old_loaded_chunks[idx] = NULL;
                        }
                    }

                    /* Copy chunk if it is still within area. */

                    if (old_local_chunk_coord.x < 0 ||
                        old_local_chunk_coord.x >= LOADED_CHUNKS_LEN ||

                        old_local_chunk_coord.y < 0 ||
                        old_local_chunk_coord.y >= LOADED_CHUNKS_LEN ||

                        old_local_chunk_coord.z < 0 ||
                        old_local_chunk_coord.z >= LOADED_CHUNKS_LEN) {
                        can_copy = false;
                    }

                    /* Copy chunk. */
                    if (can_copy) {
                        chunk_t *old =
                            old_loaded_chunks[local_chunk_coord_to_index(
                                &old_local_chunk_coord)];

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &local_chunk_coord)] = old;
                    }
                    /* Generate new chunk. */
                    else {
                        coord_t chunk_coord;

                        chunk_coord.x = local_chunk_coord.x +
                                        world->center_chunk_coord.x -
                                        RENDER_DISTANCE;
                        chunk_coord.y = local_chunk_coord.y +
                                        world->center_chunk_coord.y -
                                        RENDER_DISTANCE;
                        chunk_coord.z = local_chunk_coord.z +
                                        world->center_chunk_coord.z -
                                        RENDER_DISTANCE;

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &local_chunk_coord)] = NULL;

                        load_chunk(world, &chunk_coord);
                    }
                }
            }
        }
    }

    /* Update chunks. */
    for (i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->loaded_chunks[i]) {
            chunk_update(world->loaded_chunks[i], world);
        }
    }
}

void world_draw(world_t *world, camera_t *camera) {
    int x;
    int y;
    int z;

    glUseProgram(world->shader_program);

    glUniform1i(world->uniform_loc.texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, world->texture);

    glUniform3fv(world->uniform_loc.camera_pos, 1,
                 (const GLfloat *)camera->pos.elems);

    /* For each chunk. */
    for (x = 0; x < LOADED_CHUNKS_LEN; x++) {
        for (y = 0; y < LOADED_CHUNKS_LEN; y++) {
            for (z = 0; z < LOADED_CHUNKS_LEN; z++) {
                coord_t world_chunk_coord;
                chunk_t *chunk;
                mat4x4_t model = MAT4X4_IDENTITY;
                vec3_t translation;
                mat4x4_t model2;

                /* Translate to world coordinates. */

                world_chunk_coord.x =
                    x + world->center_chunk_coord.x - RENDER_DISTANCE;
                world_chunk_coord.y =
                    y + world->center_chunk_coord.y - RENDER_DISTANCE;
                world_chunk_coord.z =
                    z + world->center_chunk_coord.z - RENDER_DISTANCE;

                chunk = world->loaded_chunks[chunk_coord_to_index(
                    &world_chunk_coord, &world->center_chunk_coord)];

                /* Skip if not loaded. */
                if (!chunk) {
                    continue;
                }

                VEC_X(translation) = (float)(world_chunk_coord.x * CHUNK_SIZE);
                VEC_Y(translation) = (float)(world_chunk_coord.y * CHUNK_SIZE);
                VEC_Z(translation) = (float)(world_chunk_coord.z * CHUNK_SIZE);

                mat4x4_translate(&model2, &model, &translation);

                glUniformMatrix4fv(world->uniform_loc.model_matrix, 1,
                                   GL_FALSE, (const GLfloat *)model2.raw);
                glUniformMatrix4fv(world->uniform_loc.view_matrix, 1, GL_FALSE,
                                   (const GLfloat *)camera->view_matrix.raw);
                glUniformMatrix4fv(
                    world->uniform_loc.projection_matrix, 1, GL_FALSE,
                    (const GLfloat *)camera->viewport.projection_matrix.raw);

                /* Draw. */

                glBindVertexArray(chunk->vertex_array);

                glDrawElements(GL_TRIANGLES, (GLsizei)chunk->num_indices,
                               GL_UNSIGNED_INT, 0);
            }
        }
    }
}

/*void world_set_block(world_t *world, const coord_t *world_coord, u8 block) {


    coord_t chunk_coord = {{floor_div(world_coord->x, CHUNK_SIZE),
                            floor_div(world_coord->y, CHUNK_SIZE),
                            floor_div(world_coord->z, CHUNK_SIZE)}};

    coord_t local = {{world_coord->x - chunk_coord.x * CHUNK_SIZE,
                      world_coord->y - chunk_coord.y * CHUNK_SIZE,
                      world_coord->z - chunk_coord.z * CHUNK_SIZE}};

    coord_t local_chunk_coord = {
        {chunk_coord.x -
             (world->center_chunk_coord.x - RENDER_DISTANCE),
         chunk_coord.y -
             (world->center_chunk_coord.y - RENDER_DISTANCE),
         chunk_coord.z -
             (world->center_chunk_coord.z - RENDER_DISTANCE)}};

    for (int i = 0; i < 3; i++) {
        if (local_chunk_coord.elems[i] < 0 ||
            local_chunk_coord.elems[i] >= LOADED_CHUNKS_LEN)
            return;
    }

    size_t chunk_index = local_chunk_coord_to_index(&local_chunk_coord);

    chunk_t *chunk = world->loaded_chunks[chunk_index];
    if (!chunk)
        return;

    size_t block_index = local.pos.z * (CHUNK_SIZE * CHUNK_SIZE) +
                         local.pos.y * CHUNK_SIZE + local.pos.x;

    u8 old_block = chunk->blocks[block_index];
    if (old_block == block)
        return;

    chunk->blocks[block_index] = block;
    chunk->light[block_index]  = 0;
    chunk->dirty               = true;

    const int dx[6] = {-1, 1, 0, 0, 0, 0};
    const int dy[6] = {0, 0, -1, 1, 0, 0};
    const int dz[6] = {0, 0, 0, 0, -1, 1};

    for (int i = 0; i < 6; i++) {
        int nx = local.pos.x + dx[i];
        int ny = local.pos.y + dy[i];
        int nz = local.pos.z + dz[i];

        if (nx < 0 || nx >= CHUNK_SIZE || ny < 0 || ny >= CHUNK_SIZE ||
            nz < 0 || nz >= CHUNK_SIZE) {
            coord_t neighbor_chunk_coord = {{local_chunk_coord.x + dx[i],
                                             local_chunk_coord.y + dy[i],
                                             local_chunk_coord.z + dz[i]}};

            for (int j = 0; j < 3; j++) {
                if (neighbor_chunk_coord.nth[j] < 0 ||
                    neighbor_chunk_coord.nth[j] >= LOADED_CHUNKS_LEN)
                    goto skip_neighbor;
            }

            size_t nidx = local_chunk_coord_to_index(&neighbor_chunk_coord);

            chunk_t *neighbor = world->loaded_chunks[nidx];
            if (neighbor)
                neighbor->dirty = true;

        skip_neighbor:;
        }
    }

    if (old_block == BLOCK_AIR || block == BLOCK_AIR) {
        chunk->dirty = true;
    }
}*/