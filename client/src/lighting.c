/*****************************************************************************
 * File:        lighting.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "client/chunk.h"

#include <stdio.h>

#include "common/types.h"
#include "client/world.h"

static const coord_t dirs[6] = {
    /* Front. */
    {{0, 0, 1}},
    /* Back. */
    {{0, 0, -1}},
    /* Left. */
    {{-1, 0, 0}},
    /* Right. */
    {{1, 0, 0}},
    /* Top. */
    {{0, 1, 0}},
    /* Bottom. */
    {{0, -1, 0}},
};

static s64 floor_div(s64 a, s64 b) {
    s64 q = a / b;
    s64 r = a % b;
    if ((r != 0) && ((r < 0) != (b < 0))) {
        q--;
    }
    return q;
}

static int floor_mod(s64 a, s64 b) {
    s64 r = a % b;
    if (r < 0) {
        r += b;
    }
    return r;
}

typedef struct {
    coord_t block_pos;
    u8 light;
} floodfill_block_info_t;

/*static void propagate_light_from_neighbor(chunk_t *chunk, const world_t
*world, const coord_t *dir, floodfill_queue_t *floodfill_queue) { coord_t
neighbor_chunk_coord; xvec_add(chunk->coord, *dir, neighbor_chunk_coord);

    chunk_t *neighbor_chunk;
    if (!world_is_chunk_loaded(world, &neighbor_chunk_coord,
                               &neighbor_chunk)) {
        // if no chunk above, then create skylight sources
        if (dir->pos.y == 1) {
            for (s64 x = 0; x < CHUNK_SIZE; ++x) {
                for (s64 z = 0; z < CHUNK_SIZE; ++z) {
                    coord_t block_pos = {{x, CHUNK_SIZE - 1, z}};

                    size_t block_idx = INDEX_3D(block_pos.pos.x,
block_pos.pos.y, block_pos.pos.z, CHUNK_SIZE);

                    if (chunk->blocks[block_idx] != BLOCK_AIR) {
                        continue;
                    }

                    chunk->light[block_idx] = 15;

                    // push skylight onto floodfill queue
                    floodfill_block_info_t info;
                    info.block_pos = block_pos;
                    info.light     = 15;
                    xqueue_push(*floodfill_queue, info);
                }
            }
        }

        return;
    }

    if (neighbor_chunk->dirty) {
        return;
    }

    for (s64 i = 0; i < CHUNK_SIZE; ++i) {
        for (s64 j = 0; j < CHUNK_SIZE; ++j) {
            coord_t local_block_pos;

            if (dir->pos.y == 1) {
                local_block_pos.pos.x = i;
                local_block_pos.pos.y = CHUNK_SIZE - 1;
                local_block_pos.pos.z = j;
            } else if (dir->pos.y == -1) {
                local_block_pos.pos.x = i;
                local_block_pos.pos.y = 0;
                local_block_pos.pos.z = j;
            } else if (dir->pos.z == 1) {
                local_block_pos.pos.x = i;
                local_block_pos.pos.y = j;
                local_block_pos.pos.z = CHUNK_SIZE - 1;
            } else if (dir->pos.z == -1) {
                local_block_pos.pos.x = i;
                local_block_pos.pos.y = j;
                local_block_pos.pos.z = 0;
            } else if (dir->pos.x == 1) {
                local_block_pos.pos.x = CHUNK_SIZE - 1;
                local_block_pos.pos.y = i;
                local_block_pos.pos.z = j;
            } else if (dir->pos.x == -1) {
                local_block_pos.pos.x = 0;
                local_block_pos.pos.y = i;
                local_block_pos.pos.z = j;
            }

            coord_t neighbor_block_pos = local_block_pos;

            if (dir->pos.y == 1) {
                neighbor_block_pos.pos.y = 0;
            } else if (dir->pos.y == -1) {
                neighbor_block_pos.pos.y = CHUNK_SIZE - 1;
            } else if (dir->pos.z == 1) {
                neighbor_block_pos.pos.z = 0;
            } else if (dir->pos.z == -1) {
                neighbor_block_pos.pos.z = CHUNK_SIZE - 1;
            } else if (dir->pos.x == 1) {
                neighbor_block_pos.pos.x = 0;
            } else if (dir->pos.x == -1) {
                neighbor_block_pos.pos.x = CHUNK_SIZE - 1;
            }

            size_t neighbor_idx =
                INDEX_3D(neighbor_block_pos.pos.x, neighbor_block_pos.pos.y,
                      neighbor_block_pos.pos.z, CHUNK_SIZE);

            if (neighbor_chunk->blocks[neighbor_idx] != BLOCK_AIR) {
                continue;
            }

            u8 light = neighbor_chunk->light[neighbor_idx];

            if (light < 2) {
                continue;
            }

            u8 new_light = light - 1;

            // check for skylight
            if (dir->pos.y == 1) {
                if (light == 15) {
                    new_light = 15;
                }
            }

            chunk->light[INDEX_3D(local_block_pos.pos.x, local_block_pos.pos.y,
                               local_block_pos.pos.z, CHUNK_SIZE)] = new_light;

            // push light source onto floodfill queue
            floodfill_block_info_t info;
            info.block_pos = local_block_pos;
            info.light     = new_light;
            xqueue_push(*floodfill_queue, info);
        }
    }
}*/

void chunk_calculate_light(chunk_t *chunk, const world_t *world) {
    /*memset(chunk->light, 0, CHUNK_TOTAL * sizeof(u8));

    floodfill_queue_t floodfill_queue;
    xqueue_new(floodfill_queue, 4096 * 4);

    // lighting pass 1: propagate light from neighboring chunks into this one
    for (int i = 0; i < 6; ++i) {
        propagate_light_from_neighbor(chunk, world, &dirs[i],
                                      &floodfill_queue);
    }

    // lighting pass 2: bfs floodfill
    while (xqueue_len(floodfill_queue) > 0) {
        floodfill_block_info_t current = xqueue_pop(floodfill_queue);

        if (current.light == 0) {
            continue;
        }

        size_t block_idx =
            INDEX_3D(current.block_pos.pos.x, current.block_pos.pos.y,
                  current.block_pos.pos.z, CHUNK_SIZE);

        for (size_t i = 0; i < 6; ++i) {
            int nx = current.block_pos.pos.x + dirs[i].pos.x;
            int ny = current.block_pos.pos.y + dirs[i].pos.y;
            int nz = current.block_pos.pos.z + dirs[i].pos.z;

            // check if neighboring block is inside current chunk
            bool inside = nx >= 0 && nx < CHUNK_SIZE && ny >= 0 &&
                          ny < CHUNK_SIZE && nz >= 0 && nz < CHUNK_SIZE;

            if (!inside) {
                if (current.light >= 2) {
                    coord_t neighbor_chunk_coord;
                    xvec_add(chunk->coord, dirs[i], neighbor_chunk_coord);

                    chunk_t *neighbor_chunk;
                    if (world_is_chunk_loaded(world, &neighbor_chunk_coord,
                                              &neighbor_chunk)) {
                        neighbor_chunk->dirty = true;
                    }
                }

                continue;
            }

            size_t neighbor_block_idx = INDEX_3D(nx, ny, nz, CHUNK_SIZE);

            if (chunk->blocks[neighbor_block_idx] != BLOCK_AIR) {
                continue;
            }

            u8 neighbor_light     = chunk->light[neighbor_block_idx];
            u8 new_neighbor_light = current.light - 1;

            // propagate skylight downwards
            if (dirs[i].pos.y == -1 && current.light == 15) {
                new_neighbor_light = 15;
            }

            if (neighbor_light >= new_neighbor_light) {
                continue;
            }

            chunk->light[neighbor_block_idx] = new_neighbor_light;

            floodfill_block_info_t neighbor;
            neighbor.block_pos = (coord_t){{nx, ny, nz}};
            neighbor.light     = new_neighbor_light;
            xqueue_push(floodfill_queue, neighbor);
        }
    }

    xqueue_free(floodfill_queue);

    // set chunk below to dirty
    coord_t below_chunk_coord = chunk->coord;
    below_chunk_coord.pos.y--;

    chunk_t *below_chunk;
    if (world_is_chunk_loaded(world, &below_chunk_coord, &below_chunk)) {
        below_chunk->dirty = true;
    }

    // recalculate light for all dirty neighbors
    for (int i = 0; i < 6; ++i) {
        coord_t neighbor_chunk_coord;
        xvec_add(chunk->coord, dirs[i], neighbor_chunk_coord);

        chunk_t *neighbor_chunk;
        if (world_is_chunk_loaded(world, &neighbor_chunk_coord,
                                  &neighbor_chunk)) {
            if (neighbor_chunk->dirty) {
                chunk_calculate_light(neighbor_chunk, world);
            }
        }
    }*/
}

u8 chunk_get_skylight(const chunk_t *chunk, const coord_t *block_pos) {
    return (chunk->light[INDEX_3D(block_pos->pos.x, block_pos->pos.y,
                                  block_pos->pos.z, CHUNK_SIZE)] >>
            4) &
           0xf;
}

void chunk_set_skylight(chunk_t *chunk, const coord_t *block_pos,
                        u8 intensity) {
    size_t i = INDEX_3D(block_pos->pos.x, block_pos->pos.y, block_pos->pos.z,
                        CHUNK_SIZE);

    chunk->light[i] = (chunk->light[i] & 0xf) | (intensity << 4);
}

u8 chunk_get_block_light(const chunk_t *chunk, const coord_t *block_pos) {
    return chunk->light[INDEX_3D(block_pos->pos.x, block_pos->pos.y,
                                 block_pos->pos.z, CHUNK_SIZE)] &
           0xf;
}

void chunk_set_block_light(chunk_t *chunk, const coord_t *block_pos,
                           u8 intensity) {
    size_t i = INDEX_3D(block_pos->pos.x, block_pos->pos.y, block_pos->pos.z,
                        CHUNK_SIZE);

    chunk->light[i] = (chunk->light[i] & 0xf0) | intensity;
}