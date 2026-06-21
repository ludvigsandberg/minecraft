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

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common/vec_s64.h"
#include "client/chunk.h"
#include "client/gl.h"

void generate(blocks_t blocks, const vec3_s64_t *chunk_coord) {
    s64 x;
    s64 y;
    s64 z;

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                u8 *block;
                vec3_s64_t world_coord;
                f32 noise_x;
                f32 noise_y;
                f32 noise_z;
                f32 n;

                block = &blocks[z * (CHUNK_SIZE * CHUNK_SIZE) +
                                y * CHUNK_SIZE + x];

                world_coord.pos.x = chunk_coord->pos.x * CHUNK_SIZE + x;
                world_coord.pos.y = chunk_coord->pos.y * CHUNK_SIZE + y;
                world_coord.pos.z = chunk_coord->pos.z * CHUNK_SIZE + z;

                noise_x = (f32)(chunk_coord->pos.x * CHUNK_SIZE + x) / 25.0f;
                noise_y = (f32)(chunk_coord->pos.y * CHUNK_SIZE + y) / 25.0f;
                noise_z = (f32)(chunk_coord->pos.z * CHUNK_SIZE + z) / 25.0f;

                n = stb_perlin_noise3(noise_x, noise_y, noise_z, 0, 0, 0);

                if (world_coord.pos.y < -10) {
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

bool world_is_chunk_loaded(const world_t *world, const vec3_s64_t *chunk_coord,
                           chunk_t **chunk) {
    size_t i;
    chunk_t *chunk_maybe_loaded;

    for (i = 0; i < 3; i++) {
        if (chunk_coord->elems[i] <
                world->center_chunk_coord.elems[i] - RENDER_DISTANCE ||
            chunk_coord->elems[i] >
                world->center_chunk_coord.elems[i] + RENDER_DISTANCE) {
            return false;
        }
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

void world_to_local_chunk_coord(const vec3_s64_t *coord,
                                const vec3_s64_t *center,
                                vec3_s64_t *out_local) {
    size_t i;

    for (i = 0; i < 3; i++) {
        out_local->elems[i] =
            coord->elems[i] - center->elems[i] + RENDER_DISTANCE;
    }
}

size_t local_chunk_coord_to_index(const vec3_s64_t *local) {
    return local->pos.x + LOADED_CHUNKS_LEN * local->pos.y +
           LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * local->pos.z;
}

size_t chunk_coord_to_index(const vec3_s64_t *coord,
                            const vec3_s64_t *center) {
    vec3_s64_t local;
    world_to_local_chunk_coord(coord, center, &local);

    return local_chunk_coord_to_index(&local);
}

static int chunk_load_thread(void *ctx) {
    world_t *world = ctx;

    while (true) {
        chunk_job_t *job = NULL;
        chunk_result_t *res;
        size_t job_queue_size;

        SDL_LockMutex(world->mutex);

        while (world->running && xalen(world->job_queue) == 0) {
            SDL_WaitCondition(world->cond, world->mutex);
        }

        if (!world->running) {
            SDL_UnlockMutex(world->mutex);
            break;
        }

        job_queue_size = arr_chunk_job_ptr_size(&world->job_queue);

        if (job_queue_size > 0) {
            // job = world->job_queue[job_queue_size - 1];
            job = *arr_chunk_job_ptr_at(&world->job_queue, job_queue_size - 1);

            xalen(world->job_queue)--;
        }

        SDL_UnlockMutex(world->mutex);

        res        = checked_malloc(sizeof(chunk_result_t));
        res->coord = job->coord;
        generate(res->blocks, &res->coord);

        free(job);

        SDL_LockMutex(world->mutex);
        xarr_append(world->result_queue, res);
        SDL_UnlockMutex(world->mutex);
    }

    return 0;
}

static void load_chunk(world_t *world, const vec3_s64_t *chunk_coord) {
    chunk_job_t *job = malloc(sizeof(chunk_job_t));
    job->coord       = *chunk_coord;

    SDL_LockMutex(world->mutex);
    xarr_append(world->job_queue, job);
    SDL_SignalCondition(world->cond);
    SDL_UnlockMutex(world->mutex);
}

void world_new(world_t *world) {
    // load shaders

    world->shader_program =
        shader_program_new("res/chunk_vs.glsl", "res/chunk_fs.glsl");

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

    // load atlas

    struct {
        uint8_t *data;
        int width;
        int height;
    } atlas;

    atlas.data =
        stbi_load("res/atlas.png", &atlas.width, &atlas.height, NULL, 4);
    if (!atlas.data) {
        printf("Failed to load res/atlas.png: %s\n", stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &world->texture);
    glBindTexture(GL_TEXTURE_2D, world->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, atlas.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // setup chunk loading threads

    xarr_new(world->job_queue);
    xarr_new(world->result_queue);

    world->mutex = SDL_CreateMutex();
    world->cond  = SDL_CreateCondition();

    world->running = true;
    world->thread  = SDL_CreateThread(chunk_load_thread, "ChunkLoader", world);

    // load chunks

    for (size_t i = 0; i < 3; i++) {
        world->center_chunk_coord.elems[i] = 0;
    }

    memset(world->loaded_chunks, 0, LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

    vec3_s64_t chunk_coord;
    for (chunk_coord.pos.x = -RENDER_DISTANCE;
         chunk_coord.pos.x <= RENDER_DISTANCE; chunk_coord.pos.x++) {
        for (chunk_coord.pos.y = -RENDER_DISTANCE;
             chunk_coord.pos.y <= RENDER_DISTANCE; chunk_coord.pos.y++) {
            for (chunk_coord.pos.z = -RENDER_DISTANCE;
                 chunk_coord.pos.z <= RENDER_DISTANCE; chunk_coord.pos.z++) {
                load_chunk(world, &chunk_coord);
            }
        }
    }
}

void world_update(world_t *world, const camera_t *cam) {
    vec3_s64_t camera_world_chunk_coord = {{(s64)cam->pos.pos.x / CHUNK_SIZE,
                                            (s64)cam->pos.pos.y / CHUNK_SIZE,
                                            (s64)cam->pos.pos.z / CHUNK_SIZE}};

    // poll threads for new chunks

    SDL_LockMutex(world->mutex);

    for (size_t i = 0; i < xalen(world->result_queue); i++) {
        chunk_result_t *result = world->result_queue[i];

        bool chunk_within_region = true;

        for (size_t j = 0; j < 3; j++) {
            if (result->coord.nth[j] <
                    camera_world_chunk_coord.nth[j] - RENDER_DISTANCE ||
                result->coord.nth[j] >
                    camera_world_chunk_coord.nth[j] + RENDER_DISTANCE) {
                chunk_within_region = false;
                break;
            }
        }

        if (chunk_within_region) {
            chunk_t *chunk = malloc(sizeof(chunk_t));
            chunk_new(chunk, result->blocks, &result->coord, world);

            world->loaded_chunks[chunk_coord_to_index(
                &result->coord, &world->center_chunk_coord)] = chunk;
        }

        free(result);
    }

    xalen(world->result_queue) = 0;

    SDL_UnlockMutex(world->mutex);

    // check if camera has moved to a different chunk

    bool camera_moved_to_different_chunk = false;

    for (size_t i = 0; i < 3; i++) {
        if (world->center_chunk_coord.elems[i] !=
            camera_world_chunk_coord.elems[i]) {
            camera_moved_to_different_chunk = true;
            break;
        }
    }

    // move and generate chunks
    if (camera_moved_to_different_chunk) {
        vec3_s64_t old_center_chunk_coord = world->center_chunk_coord;
        world->center_chunk_coord         = camera_world_chunk_coord;

        vec3_s64_t chunk_coord_diff = {
            {world->center_chunk_coord.pos.x - old_center_chunk_coord.pos.x,
             world->center_chunk_coord.pos.y - old_center_chunk_coord.pos.y,
             world->center_chunk_coord.pos.z - old_center_chunk_coord.pos.z}};

        chunk_t *old_loaded_chunks[LOADED_CHUNKS_TOTAL];
        memcpy(old_loaded_chunks, world->loaded_chunks,
               LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

        // for each chunk in the new region, check if we can copy the chunk
        // from the previously loaded region or if a new one has to be
        // generated

        for (s64 x = 0; x < LOADED_CHUNKS_LEN; x++) {
            for (s64 y = 0; y < LOADED_CHUNKS_LEN; y++) {
                for (s64 z = 0; z < LOADED_CHUNKS_LEN; z++) {
                    vec3_s64_t local_chunk_coord     = {{x, y, z}};
                    vec3_s64_t old_local_chunk_coord = {
                        {x + chunk_coord_diff.pos.x,
                         y + chunk_coord_diff.pos.y,
                         z + chunk_coord_diff.pos.z}};

                    // check if chunk has moved out of new area
                    // free if so

                    bool delete = false;
                    for (size_t i = 0; i < 3; i++) {
                        if (local_chunk_coord.elems[i] <
                                chunk_coord_diff.elems[i] ||
                            local_chunk_coord.elems[i] >=
                                chunk_coord_diff.elems[i] +
                                    LOADED_CHUNKS_LEN) {
                            delete = true;
                            break;
                        }
                    }

                    if (delete) {
                        size_t idx =
                            local_chunk_coord_to_index(&local_chunk_coord);

                        if (old_loaded_chunks[idx]) {
                            // set chunk below to dirty

                            vec3_s64_t below_chunk_coord =
                                old_loaded_chunks[idx]->coord;
                            below_chunk_coord.xyz.y--;

                            chunk_t *below_chunk;

                            if (world_is_chunk_loaded(
                                    world, &below_chunk_coord, &below_chunk)) {
                                below_chunk->dirty = true;
                            }

                            // free
                            chunk_free(old_loaded_chunks[idx]);
                            free(old_loaded_chunks[idx]);
                            old_loaded_chunks[idx] = NULL;
                        }
                    }

                    // check if chunk is still within area and can be copied

                    bool can_copy = true;
                    for (size_t i = 0; i < 3; i++) {
                        if (old_local_chunk_coord.elems[i] < 0 ||
                            old_local_chunk_coord.elems[i] >=
                                LOADED_CHUNKS_LEN) {
                            can_copy = false;
                            break;
                        }
                    }

                    // copy chunk
                    if (can_copy) {
                        chunk_t *old =
                            old_loaded_chunks[local_chunk_coord_to_index(
                                &old_local_chunk_coord)];

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &local_chunk_coord)] = old;
                    }
                    // generate new chunk
                    else {
                        vec3_s64_t chunk_coord;
                        for (size_t i = 0; i < 3; i++) {
                            chunk_coord.elems[i] =
                                local_chunk_coord.elems[i] +
                                world->center_chunk_coord.elems[i] -
                                RENDER_DISTANCE;
                        }

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &local_chunk_coord)] = NULL;

                        load_chunk(world, &chunk_coord);
                    }
                }
            }
        }
    }

    // update chunks
    for (size_t i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->loaded_chunks[i]) {
            chunk_update(world->loaded_chunks[i], world);
        }
    }
}

void world_draw(world_t *world, camera_t *camera) {
    glUseProgram(world->shader_program);

    glUniform1i(world->uniform_loc.texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, world->texture);

    glUniform3fv(world->uniform_loc.camera_pos, 1,
                 (const GLfloat *)camera->pos.nth);

    // for each chunk
    for (int x = 0; x < LOADED_CHUNKS_LEN; x++) {
        for (int y = 0; y < LOADED_CHUNKS_LEN; y++) {
            for (int z = 0; z < LOADED_CHUNKS_LEN; z++) {
                // translate to world coordinates

                vec3_s64_t world_chunk_coord = {
                    {x + world->center_chunk_coord.pos.x - RENDER_DISTANCE,
                     y + world->center_chunk_coord.pos.y - RENDER_DISTANCE,
                     z + world->center_chunk_coord.pos.z - RENDER_DISTANCE}};

                chunk_t *chunk = world->loaded_chunks[chunk_coord_to_index(
                    &world_chunk_coord, &world->center_chunk_coord)];

                // skip if not loaded
                if (!chunk) {
                    continue;
                }

                xmat4f32_t model;
                xmat_identity_f32(model);
                xvec3f32_t translation = {
                    {(float)(world_chunk_coord.pos.x * CHUNK_SIZE),
                     (float)(world_chunk_coord.pos.y * CHUNK_SIZE),
                     (float)(world_chunk_coord.pos.z * CHUNK_SIZE)}};
                xmat4f32_t model2;
                xmat4f32_translate(model, translation, model2);

                glUniformMatrix4fv(world->uniform_loc.model_matrix, 1,
                                   GL_FALSE, (const GLfloat *)model2.nth);
                glUniformMatrix4fv(world->uniform_loc.view_matrix, 1, GL_FALSE,
                                   (const GLfloat *)camera->view_matrix.nth);
                glUniformMatrix4fv(
                    world->uniform_loc.projection_matrix, 1, GL_FALSE,
                    (const GLfloat *)camera->viewport.projection_matrix.nth);

                // draw

                glBindVertexArray(chunk->vertex_array);

                glDrawElements(GL_TRIANGLES, chunk->num_indices,
                               GL_UNSIGNED_INT, 0);
            }
        }
    }
}

static int floor_div(int a, int b) {
    return (a >= 0) ? (a / b) : ((a - b + 1) / b);
}

void world_set_block(world_t *world, const vec3_s64_t *world_coord,
                     uint8_t block) {
    vec3_s64_t chunk_coord = {{floor_div(world_coord->pos.x, CHUNK_SIZE),
                               floor_div(world_coord->pos.y, CHUNK_SIZE),
                               floor_div(world_coord->pos.z, CHUNK_SIZE)}};

    vec3_s64_t local = {{world_coord->pos.x - chunk_coord.pos.x * CHUNK_SIZE,
                         world_coord->pos.y - chunk_coord.pos.y * CHUNK_SIZE,
                         world_coord->pos.z - chunk_coord.pos.z * CHUNK_SIZE}};

    vec3_s64_t local_chunk_coord = {
        {chunk_coord.pos.x -
             (world->center_chunk_coord.pos.x - RENDER_DISTANCE),
         chunk_coord.pos.y -
             (world->center_chunk_coord.pos.y - RENDER_DISTANCE),
         chunk_coord.pos.z -
             (world->center_chunk_coord.pos.z - RENDER_DISTANCE)}};

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

    uint8_t old_block = chunk->blocks[block_index];
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
            vec3_s64_t neighbor_chunk_coord = {
                {local_chunk_coord.pos.x + dx[i],
                 local_chunk_coord.pos.y + dy[i],
                 local_chunk_coord.pos.z + dz[i]}};

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
}