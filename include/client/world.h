#ifndef WORLD_H
#define WORLD_H

#include <pthread.h>

#include "coord.h"
#include "client/opengl.h"
#include "client/chunk.h"
#include "client/renderer.h"
#include "client/camera.h"

#define RENDER_DISTANCE   4
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

typedef struct world_s {
    chunk_coord_t center;

    /* 3D array of pointers to loaded chunks, NULL for unloaded chunks. */
    chunk_t *loaded_chunks[LOADED_CHUNKS_TOTAL];

    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int is_running;

    chunk_job_t **job_queue;
    size_t job_queue_count;
    size_t job_queue_cap;

    chunk_result_t **result_queue;
    size_t result_queue_count;
    size_t result_queue_cap;
} world_t;

void world_init(world_t *world);
void world_update(world_t *world, const camera_t *cam);
void world_draw(world_t *world, const renderer_t *renderer, camera_t *camera);

#endif
