#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <stdbool.h>

#include <tinycthread.h>
#include <meta.h>

#include <chunk.h>
#include <camera.h>

#define RENDER_DISTANCE   4
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN *LOADED_CHUNKS_LEN *LOADED_CHUNKS_LEN

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

    thrd_t thread;
    mtx_t mutex;
    cnd_t cond;
    bool running;

    arr(chunk_job_t *) job_queue;
    arr(chunk_result_t *) result_queue;
} world_t;

size_t chunk_coord_to_index(const coord_t coord, const coord_t center);

void world_new(world_t *world);
void world_update(world_t *world, const camera_t *cam);

#endif
