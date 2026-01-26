#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <stdbool.h>

#include <SDL3/SDL_thread.h>
#include <x/vec.h>
#include <x/arr.h>

#include <minecraft/gl.h>
#include <minecraft/chunk.h>
#include <minecraft/camera.h>

#define RENDER_DISTANCE   2
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

typedef struct {
    xvec3i64_t coord;
} chunk_job_t;

typedef struct {
    xvec3i64_t coord;
    blocks_t blocks;
} chunk_result_t;

typedef struct {
    xvec3i64_t center_chunk_coord;
    chunk_t *loaded_chunks[LOADED_CHUNKS_TOTAL];

    SDL_Thread *thread;
    SDL_Mutex *mutex;
    SDL_Condition *cond;
    bool running;
    xarr(chunk_job_t *) job_queue;
    xarr(chunk_result_t *) result_queue;

    GLuint shader_program;
    GLuint texture;

    struct {
        GLint texture;
        GLint model_matrix;
        GLint view_matrix;
        GLint projection_matrix;
        GLint camera_pos;
    } uniform_loc;
} world_t;

// check if chunk coord is within bounds and check if chunk is loaded
bool world_is_chunk_loaded(const world_t *world, const xvec3i64_t *chunk_coord,
                           chunk_t **chunk);

void world_to_local_chunk_coord(const xvec3i64_t *coord,
                                const xvec3i64_t *center,
                                xvec3i64_t *out_local);

size_t local_chunk_coord_to_index(const xvec3i64_t *local);

size_t chunk_coord_to_index(const xvec3i64_t *coord, const xvec3i64_t *center);

void world_new(world_t *world);
void world_update(world_t *world, const camera_t *cam);
void world_draw(world_t *world, camera_t *camera);
void world_set_block(world_t *world, const xvec3i64_t *world_coord,
                     uint8_t block);

#endif
