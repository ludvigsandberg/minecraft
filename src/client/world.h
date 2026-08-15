#ifndef WORLD_H
#define WORLD_H

#include <pthread.h>

#include "array.h"
#include "map.h"
#include "ring_buffer.h"
#include "coord.h"
#include "client/chunk.h"
#include "client/camera.h"

#define RENDER_DISTANCE   4
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN * LOADED_CHUNKS_LEN

struct world;

/* Task context for loading chunks within (and unloading chunks outside) render
   distance. */
struct sweep_context {
    struct coord center;
    struct world *world;
};

/* Chunk block generation task context. */
struct gen_context {
    struct coord coord;
    const struct world *world;
};

/* Chunk block generation task result. */
struct gen_result {
    struct coord coord;
    unsigned char blocks[CHUNK_TOTAL];
};

/* Chunk meshing task context. */
struct mesh_context {
    struct coord coord;
    const struct world *world;
};

/* Chunk meshing task result. */
struct mesh_result {
    struct coord coord;

    struct array vertices;
    float *vertex_elems;

    struct array indices;
    unsigned int *index_elems;
};

struct chunk_entry {
    struct coord key;
    struct chunk *value;
    enum map_slot slot;
};

struct world {
    /* Spatial hash-map of coordinates to loaded chunks. */
    struct map chunks;
    struct chunk_entry *chunk_entries;

    /* Task result mutex. */
    pthread_mutex_t mutex;

    /* Task results. */

    struct ring_buffer gens;
    struct gen_result **gen_elems;

    struct ring_buffer meshes;
    struct mesh_result **mesh_elems;

    /* TRUE if a sweep task is enqueued or being processed. */
    int is_sweep_active;
};

void world_init(struct world *world);
void world_update(struct world *world, const struct camera *camera);

#endif
