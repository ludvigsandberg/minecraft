#ifndef WORLD_H
#define WORLD_H

#include <pthread.h>

#include "queue.h"
#include "coord.h"
#include "client/opengl.h"
#include "client/chunk.h"
#include "client/renderer.h"
#include "client/camera.h"

#define RENDER_DISTANCE   4
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

struct world {
    struct coord loaded_chunks_center;

    /* 3D array of pointers to loaded chunks, NULL for unloaded chunks. */
    struct chunk **loaded_chunks;
    struct array_info loaded_chunks_info;

    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int is_running;

    struct chunk_job **chunk_jobs;
    struct queue_info chunk_jobs_info;

    struct chunk_result **chunk_results;
    struct queue_info chunk_results_info;
};

void world_init(struct world *world);
void world_update(struct world *world, const struct camera *camera);
void world_draw(struct world *world, const renderer_t *renderer,
                struct camera *camera);

#endif
