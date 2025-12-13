#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <stdbool.h>

#include <SDL3/SDL_thread.h>
#include <meta.h>

#include <minecraft/gl.h>
#include <minecraft/chunk.h>
#include <minecraft/camera.h>

#define RENDER_DISTANCE   6
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

typedef int64_t coord_t[3];

typedef struct {
    coord_t coord;
} chunk_job_t;

typedef struct {
    coord_t coord;
    blocks_t blocks;
} chunk_result_t;

typedef struct {
    coord_t center_chunk_coord;
    chunk_t *loaded_chunks[LOADED_CHUNKS_TOTAL];

    SDL_Thread *thread;
    SDL_Mutex *mutex;
    SDL_Condition *cond;
    bool running;
    arr(chunk_job_t *) job_queue;
    arr(chunk_result_t *) result_queue;

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

size_t chunk_coord_to_index(const coord_t coord, const coord_t center);

void world_new(world_t *world);
void world_update(world_t *world, const camera_t *cam);
void world_draw(world_t *world, camera_t *camera);
void world_set_block(world_t *world, coord_t world_coord, uint8_t block);

#endif
