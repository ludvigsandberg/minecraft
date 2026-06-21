/*****************************************************************************
 * File:        world.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_WORLD_H
#define CLIENT_WORLD_H

#include <SDL3/SDL_thread.h>

#include "common/vec_s64.h"
#include "client/gl.h"
#include "client/chunk.h"
#include "client/camera.h"
#include "client/arr_chunk_job_ptr.h"
#include "client/arr_chunk_result_ptr.h"

#define RENDER_DISTANCE   2
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

typedef struct {
    vec3_s64_t center_chunk_coord;
    /* 3D array of pointers to loaded chunks or NULL for unloaded chunks. */
    chunk_t *loaded_chunks[LOADED_CHUNKS_TOTAL];

    SDL_Thread *thread;
    SDL_Mutex *mutex;
    SDL_Condition *cond;
    bool running;
    arr_chunk_job_ptr_t job_queue; /* Array of pointers to chunk jobs. */
    arr_chunk_result_ptr_t
        result_queue; /* Array of pointers to chunk results. */

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
bool world_is_chunk_loaded(const world_t *world, const vec3_s64_t *chunk_coord,
                           chunk_t **chunk);

void world_to_local_chunk_coord(const vec3_s64_t *coord,
                                const vec3_s64_t *center,
                                vec3_s64_t *out_local);

size_t local_chunk_coord_to_index(const vec3_s64_t *local);

size_t chunk_coord_to_index(const vec3_s64_t *coord, const vec3_s64_t *center);

void world_new(world_t *world);
void world_update(world_t *world, const camera_t *cam);
void world_draw(world_t *world, camera_t *camera);
void world_set_block(world_t *world, const vec3_s64_t *world_coord, u8 block);

#endif
