#ifndef WORLD_H
#define WORLD_H

#include <chunk.h>
#include <stdint.h>

#define RENDER_DISTANCE   3
#define LOADED_CHUNKS_LEN (RENDER_DISTANCE * 2 + 1)
#define LOADED_CHUNKS_TOTAL                                                   \
    LOADED_CHUNKS_LEN *LOADED_CHUNKS_LEN *LOADED_CHUNKS_LEN

typedef int64_t coord_t[3];

typedef struct {
    coord_t chunk_center_coord;
    chunk_t *loaded_chunks[LOADED_CHUNKS_TOTAL];
} world_t;

size_t world_chunk_coord_to_index(const world_t *world,
                                  const coord_t chunk_coord);

void world_new(world_t *world);
void world_free(world_t *world);
void world_update(world_t *world);

#endif
