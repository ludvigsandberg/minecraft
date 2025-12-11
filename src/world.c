#include <world.h>

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

#include <chunk.h>
#include <gl.h>

void generate(blocks_t blocks, coord_t chunk_coord) {
    for (int64_t x = 0; x < CHUNK_SIZE; x++) {
        for (int64_t y = 0; y < CHUNK_SIZE; y++) {
            for (int64_t z = 0; z < CHUNK_SIZE; z++) {
                uint8_t *block = &blocks[z * (CHUNK_SIZE * CHUNK_SIZE) +
                                         y * CHUNK_SIZE + x];

                coord_t world_coord = {chunk_coord[0] * CHUNK_SIZE + x,
                                       chunk_coord[1] * CHUNK_SIZE + y,
                                       chunk_coord[2] * CHUNK_SIZE + z};

                float noise_x =
                    (float)(chunk_coord[0] * CHUNK_SIZE + x) / 25.0f;
                float noise_y =
                    (float)(chunk_coord[1] * CHUNK_SIZE + y) / 25.0f;
                float noise_z =
                    (float)(chunk_coord[2] * CHUNK_SIZE + z) / 25.0f;

                float n =
                    stb_perlin_noise3(noise_x, noise_y, noise_z, 0, 0, 0);

                if (n > .3f) {
                    *block = n > .31f ? BLOCK_STONE : BLOCK_GRASS;
                } else {
                    *block = BLOCK_AIR;
                }
            }
        }
    }
}

static void world_to_local_chunk_coord(const coord_t coord,
                                       const coord_t center,
                                       coord_t out_local) {
    for (size_t i = 0; i < 3; i++) {
        out_local[i] = coord[i] - center[i] + RENDER_DISTANCE;
    }
}

static size_t local_chunk_coord_to_index(const coord_t local) {
    return local[0] + LOADED_CHUNKS_LEN * local[1] +
           LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * local[2];
}

size_t chunk_coord_to_index(const coord_t coord, const coord_t center) {
    coord_t local;
    world_to_local_chunk_coord(coord, center, local);

    return local_chunk_coord_to_index(local);
}

static int chunk_load_thread(void *ctx) {
    world_t *world = ctx;

    while (true) {
        SDL_LockMutex(world->mutex);

        while (world->running && alen(world->job_queue) == 0) {
            SDL_WaitCondition(world->cond, world->mutex);
        }

        if (!world->running) {
            SDL_UnlockMutex(world->mutex);
            break;
        }

        chunk_job_t *job = NULL;
        if (alen(world->job_queue) > 0) {
            job = world->job_queue[alen(world->job_queue) - 1];
            alen(world->job_queue)--;
        }

        SDL_UnlockMutex(world->mutex);

        chunk_result_t *res = malloc(sizeof(chunk_result_t));
        memcpy(res->coord, job->coord, sizeof(coord_t));
        generate(res->blocks, res->coord);

        free(job);

        SDL_LockMutex(world->mutex);
        arr_append(world->result_queue, res);
        SDL_UnlockMutex(world->mutex);
    }

    return 0;
}

static void load_chunk(world_t *world, coord_t chunk_coord) {
    chunk_job_t *job = malloc(sizeof(chunk_job_t));
    memcpy(job->coord, chunk_coord, sizeof(coord_t));

    SDL_LockMutex(world->mutex);
    arr_append(world->job_queue, job);
    SDL_SignalCondition(world->cond);
    SDL_UnlockMutex(world->mutex);
}

void world_new(world_t *world) {
    // Shaders.

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

    // Texture atlas.

    struct {
        uint8_t *data;
        int width;
        int height;
        int channels;
    } atlas;

    atlas.data = stbi_load("res/atlas.png", &atlas.width, &atlas.height,
                           &atlas.channels, 4);
    if (!atlas.data) {
        printf("Failed to load res/atlas.png: %s\n", stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &world->texture);
    glBindTexture(GL_TEXTURE_2D, world->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, atlas.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Setup chunk loader threads.

    arr_new(world->job_queue);
    arr_new(world->result_queue);

    world->mutex = SDL_CreateMutex();
    world->cond  = SDL_CreateCondition();

    world->running = true;
    world->thread  = SDL_CreateThread(chunk_load_thread, "ChunkLoader", world);

    // Load chunks.

    for (size_t i = 0; i < 3; i++) {
        world->center_chunk_coord[i] = 0;
    }

    memset(world->loaded_chunks, 0, LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

    coord_t chunk_coord;
    for (chunk_coord[0] = -RENDER_DISTANCE; chunk_coord[0] <= RENDER_DISTANCE;
         chunk_coord[0]++) {
        for (chunk_coord[1] = -RENDER_DISTANCE;
             chunk_coord[1] <= RENDER_DISTANCE; chunk_coord[1]++) {
            for (chunk_coord[2] = -RENDER_DISTANCE;
                 chunk_coord[2] <= RENDER_DISTANCE; chunk_coord[2]++) {
                load_chunk(world, chunk_coord);
            }
        }
    }
}

void world_update(world_t *world, const camera_t *cam) {
    coord_t camera_world_chunk_coord = {
        camera_world_chunk_coord[0] = (int64_t)cam->pos[0] / CHUNK_SIZE,
        camera_world_chunk_coord[1] = (int64_t)cam->pos[1] / CHUNK_SIZE,
        camera_world_chunk_coord[2] = (int64_t)cam->pos[2] / CHUNK_SIZE};

    // Poll chunk thread for new chunks.

    SDL_LockMutex(world->mutex);

    for (size_t i = 0; i < alen(world->result_queue); i++) {
        chunk_result_t *result = world->result_queue[i];

        bool chunk_within_region = true;

        for (size_t j = 0; j < 3; j++) {
            if (result->coord[j] <
                    camera_world_chunk_coord[j] - RENDER_DISTANCE ||
                result->coord[j] >
                    camera_world_chunk_coord[j] + RENDER_DISTANCE) {
                chunk_within_region = false;
                break;
            }
        }

        if (chunk_within_region) {
            chunk_t *chunk = malloc(sizeof(chunk_t));
            chunk_new(chunk, result->blocks);

            world->loaded_chunks[chunk_coord_to_index(
                result->coord, world->center_chunk_coord)] = chunk;
        }

        free(result);
    }

    alen(world->result_queue) = 0;

    SDL_UnlockMutex(world->mutex);

    // Check if camera has moved to a different chunk.

    bool camera_moved_to_different_chunk = false;

    for (size_t i = 0; i < 3; i++) {
        if (world->center_chunk_coord[i] != camera_world_chunk_coord[i]) {
            camera_moved_to_different_chunk = true;
            break;
        }
    }

    if (!camera_moved_to_different_chunk) {
        return;
    }

    // Move loaded chunks and generate new ones.

    coord_t old_center_chunk_coord;
    memcpy(old_center_chunk_coord, world->center_chunk_coord, sizeof(coord_t));

    memcpy(world->center_chunk_coord, camera_world_chunk_coord,
           sizeof(coord_t));

    coord_t chunk_coord_diff = {
        world->center_chunk_coord[0] - old_center_chunk_coord[0],
        world->center_chunk_coord[1] - old_center_chunk_coord[1],
        world->center_chunk_coord[2] - old_center_chunk_coord[2]};

    chunk_t *old_loaded_chunks[LOADED_CHUNKS_TOTAL];
    memcpy(old_loaded_chunks, world->loaded_chunks,
           LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

    // For each chunk in the new region, check if we can copy the chunk from
    // the previously loaded region or if we have to generate a new one.

    for (int64_t x = 0; x < LOADED_CHUNKS_LEN; x++) {
        for (int64_t y = 0; y < LOADED_CHUNKS_LEN; y++) {
            for (int64_t z = 0; z < LOADED_CHUNKS_LEN; z++) {
                coord_t local_chunk_coord     = {x, y, z};
                coord_t old_local_chunk_coord = {x + chunk_coord_diff[0],
                                                 y + chunk_coord_diff[1],
                                                 z + chunk_coord_diff[2]};

                // Check if chunk has moved out of new area and free if so.

                bool delete = false;
                for (size_t i = 0; i < 3; i++) {
                    if (local_chunk_coord[i] < chunk_coord_diff[i] ||
                        local_chunk_coord[i] >=
                            chunk_coord_diff[i] + LOADED_CHUNKS_LEN) {
                        delete = true;
                        break;
                    }
                }

                if (delete) {
                    size_t idx = local_chunk_coord_to_index(local_chunk_coord);

                    if (old_loaded_chunks[idx]) {
                        chunk_free(*old_loaded_chunks[idx]);
                        free(old_loaded_chunks[idx]);
                        old_loaded_chunks[idx] = NULL;
                    }
                }

                // Check if chunk is still within area and can be copied.

                bool can_copy = true;
                for (size_t i = 0; i < 3; i++) {
                    if (old_local_chunk_coord[i] < 0 ||
                        old_local_chunk_coord[i] >= LOADED_CHUNKS_LEN) {
                        can_copy = false;
                        break;
                    }
                }

                // Can copy chunk.
                if (can_copy) {
                    chunk_t *old =
                        old_loaded_chunks[local_chunk_coord_to_index(
                            old_local_chunk_coord)];

                    world->loaded_chunks[local_chunk_coord_to_index(
                        local_chunk_coord)] = old;

                }
                // Generate new chunk.
                else {
                    coord_t chunk_coord;
                    for (size_t i = 0; i < 3; i++) {
                        chunk_coord[i] = local_chunk_coord[i] +
                                         world->center_chunk_coord[i] -
                                         RENDER_DISTANCE;
                    }

                    world->loaded_chunks[local_chunk_coord_to_index(
                        local_chunk_coord)] = NULL;

                    load_chunk(world, chunk_coord);
                }
            }
        }
    }
}

void world_draw(world_t *world, camera_t *camera) {
    // Bind & draw.

    glUseProgram(world->shader_program);

    glUniform1i(world->uniform_loc.texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, world->texture);

    glUniform3fv(world->uniform_loc.camera_pos, 1,
                 (const GLfloat *)camera->pos);

    // Draw loaded chunks.

    for (int x = 0; x < LOADED_CHUNKS_LEN; x++) {
        for (int y = 0; y < LOADED_CHUNKS_LEN; y++) {
            for (int z = 0; z < LOADED_CHUNKS_LEN; z++) {
                // Translate loaded chunks to world coordinates.

                coord_t world_chunk_coord = {
                    x + world->center_chunk_coord[0] - RENDER_DISTANCE,
                    y + world->center_chunk_coord[1] - RENDER_DISTANCE,
                    z + world->center_chunk_coord[2] - RENDER_DISTANCE};

                mat4x4 model;
                mat4x4_identity(model);
                mat4x4_translate_in_place(
                    model, (float)(world_chunk_coord[0] * CHUNK_SIZE),
                    (float)(world_chunk_coord[1] * CHUNK_SIZE),
                    (float)(world_chunk_coord[2] * CHUNK_SIZE));

                glUniformMatrix4fv(world->uniform_loc.model_matrix, 1,
                                   GL_FALSE, (const GLfloat *)model);
                glUniformMatrix4fv(world->uniform_loc.view_matrix, 1, GL_FALSE,
                                   (const GLfloat *)camera->view_matrix);
                glUniformMatrix4fv(
                    world->uniform_loc.projection_matrix, 1, GL_FALSE,
                    (const GLfloat *)camera->viewport.projection_matrix);

                // Draw.

                chunk_t *chunk = world->loaded_chunks[chunk_coord_to_index(
                    world_chunk_coord, world->center_chunk_coord)];

                if (!chunk) {
                    continue;
                }

                glBindVertexArray(chunk->vertex_array);

                glDrawElements(GL_TRIANGLES, chunk->num_indices,
                               GL_UNSIGNED_INT, 0);
            }
        }
    }
}
