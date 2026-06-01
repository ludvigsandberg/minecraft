#include <minecraft/chunk.h>

#include <stdio.h>

#include <x/queue.h>

#include <minecraft/world.h>

static const xvec3i64_t dirs[6] = {
    // front
    {{0, 0, 1}},
    // back
    {{0, 0, -1}},
    // left
    {{-1, 0, 0}},
    // right
    {{1, 0, 0}},
    // top
    {{0, 1, 0}},
    // bottom
    {{0, -1, 0}},
};

static int64_t floor_div(int64_t a, int64_t b) {
    int64_t q = a / b;
    int64_t r = a % b;
    if ((r != 0) && ((r < 0) != (b < 0))) {
        q--;
    }
    return q;
}

static int floor_mod(int64_t a, int64_t b) {
    int64_t r = a % b;
    if (r < 0) {
        r += b;
    }
    return r;
}

typedef struct {
    xvec3i64_t block_pos;
    uint8_t light;
} floodfill_block_info_t;

typedef xqueue(floodfill_block_info_t) floodfill_queue_t;

static void propagate_light_from_neighbor(chunk_t *chunk, const world_t *world,
                                          const xvec3i64_t *dir,
                                          floodfill_queue_t *floodfill_queue) {
    xvec3i64_t neighbor_chunk_coord;
    xvec_add(chunk->coord, *dir, neighbor_chunk_coord);

    chunk_t *neighbor_chunk;
    if (!world_is_chunk_loaded(world, &neighbor_chunk_coord,
                               &neighbor_chunk)) {
        // if no chunk above, then create skylight sources
        if (dir->xyz.y == 1) {
            for (int64_t x = 0; x < CHUNK_SIZE; ++x) {
                for (int64_t z = 0; z < CHUNK_SIZE; ++z) {
                    xvec3i64_t block_pos = {{x, CHUNK_SIZE - 1, z}};

                    size_t block_idx = idx3d(block_pos.xyz.x, block_pos.xyz.y,
                                             block_pos.xyz.z, CHUNK_SIZE);

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

    for (int64_t i = 0; i < CHUNK_SIZE; ++i) {
        for (int64_t j = 0; j < CHUNK_SIZE; ++j) {
            xvec3i64_t local_block_pos;

            if (dir->xyz.y == 1) {
                local_block_pos.xyz.x = i;
                local_block_pos.xyz.y = CHUNK_SIZE - 1;
                local_block_pos.xyz.z = j;
            } else if (dir->xyz.y == -1) {
                local_block_pos.xyz.x = i;
                local_block_pos.xyz.y = 0;
                local_block_pos.xyz.z = j;
            } else if (dir->xyz.z == 1) {
                local_block_pos.xyz.x = i;
                local_block_pos.xyz.y = j;
                local_block_pos.xyz.z = CHUNK_SIZE - 1;
            } else if (dir->xyz.z == -1) {
                local_block_pos.xyz.x = i;
                local_block_pos.xyz.y = j;
                local_block_pos.xyz.z = 0;
            } else if (dir->xyz.x == 1) {
                local_block_pos.xyz.x = CHUNK_SIZE - 1;
                local_block_pos.xyz.y = i;
                local_block_pos.xyz.z = j;
            } else if (dir->xyz.x == -1) {
                local_block_pos.xyz.x = 0;
                local_block_pos.xyz.y = i;
                local_block_pos.xyz.z = j;
            }

            xvec3i64_t neighbor_block_pos = local_block_pos;

            if (dir->xyz.y == 1) {
                neighbor_block_pos.xyz.y = 0;
            } else if (dir->xyz.y == -1) {
                neighbor_block_pos.xyz.y = CHUNK_SIZE - 1;
            } else if (dir->xyz.z == 1) {
                neighbor_block_pos.xyz.z = 0;
            } else if (dir->xyz.z == -1) {
                neighbor_block_pos.xyz.z = CHUNK_SIZE - 1;
            } else if (dir->xyz.x == 1) {
                neighbor_block_pos.xyz.x = 0;
            } else if (dir->xyz.x == -1) {
                neighbor_block_pos.xyz.x = CHUNK_SIZE - 1;
            }

            size_t neighbor_idx =
                idx3d(neighbor_block_pos.xyz.x, neighbor_block_pos.xyz.y,
                      neighbor_block_pos.xyz.z, CHUNK_SIZE);

            if (neighbor_chunk->blocks[neighbor_idx] != BLOCK_AIR) {
                continue;
            }

            uint8_t light = neighbor_chunk->light[neighbor_idx];

            if (light < 2) {
                continue;
            }

            uint8_t new_light = light - 1;

            // check for skylight
            if (dir->xyz.y == 1) {
                if (light == 15) {
                    new_light = 15;
                }
            }

            chunk->light[idx3d(local_block_pos.xyz.x, local_block_pos.xyz.y,
                               local_block_pos.xyz.z, CHUNK_SIZE)] = new_light;

            // push light source onto floodfill queue
            floodfill_block_info_t info;
            info.block_pos = local_block_pos;
            info.light     = new_light;
            xqueue_push(*floodfill_queue, info);
        }
    }
}

void chunk_calculate_light(chunk_t *chunk, const world_t *world) {
    memset(chunk->light, 0, CHUNK_TOTAL * sizeof(uint8_t));

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
            idx3d(current.block_pos.xyz.x, current.block_pos.xyz.y,
                  current.block_pos.xyz.z, CHUNK_SIZE);

        for (size_t i = 0; i < 6; ++i) {
            int nx = current.block_pos.xyz.x + dirs[i].xyz.x;
            int ny = current.block_pos.xyz.y + dirs[i].xyz.y;
            int nz = current.block_pos.xyz.z + dirs[i].xyz.z;

            // check if neighboring block is inside current chunk
            bool inside = nx >= 0 && nx < CHUNK_SIZE && ny >= 0 &&
                          ny < CHUNK_SIZE && nz >= 0 && nz < CHUNK_SIZE;

            if (!inside) {
                if (current.light >= 2) {
                    xvec3i64_t neighbor_chunk_coord;
                    xvec_add(chunk->coord, dirs[i], neighbor_chunk_coord);

                    chunk_t *neighbor_chunk;
                    if (world_is_chunk_loaded(world, &neighbor_chunk_coord,
                                              &neighbor_chunk)) {
                        neighbor_chunk->dirty = true;
                    }
                }

                continue;
            }

            size_t neighbor_block_idx = idx3d(nx, ny, nz, CHUNK_SIZE);

            if (chunk->blocks[neighbor_block_idx] != BLOCK_AIR) {
                continue;
            }

            uint8_t neighbor_light     = chunk->light[neighbor_block_idx];
            uint8_t new_neighbor_light = current.light - 1;

            // propagate skylight downwards
            if (dirs[i].xyz.y == -1 && current.light == 15) {
                new_neighbor_light = 15;
            }

            if (neighbor_light >= new_neighbor_light) {
                continue;
            }

            chunk->light[neighbor_block_idx] = new_neighbor_light;

            floodfill_block_info_t neighbor;
            neighbor.block_pos = (xvec3i64_t){{nx, ny, nz}};
            neighbor.light     = new_neighbor_light;
            xqueue_push(floodfill_queue, neighbor);
        }
    }

    xqueue_free(floodfill_queue);

    // set chunk below to dirty
    xvec3i64_t below_chunk_coord = chunk->coord;
    below_chunk_coord.xyz.y--;

    chunk_t *below_chunk;
    if (world_is_chunk_loaded(world, &below_chunk_coord, &below_chunk)) {
        below_chunk->dirty = true;
    }

    // recalculate light for all dirty neighbors
    for (int i = 0; i < 6; ++i) {
        xvec3i64_t neighbor_chunk_coord;
        xvec_add(chunk->coord, dirs[i], neighbor_chunk_coord);

        chunk_t *neighbor_chunk;
        if (world_is_chunk_loaded(world, &neighbor_chunk_coord,
                                  &neighbor_chunk)) {
            if (neighbor_chunk->dirty) {
                chunk_calculate_light(neighbor_chunk, world);
            }
        }
    }
}

uint8_t chunk_get_skylight(const chunk_t *chunk, const xvec3i64_t *block_pos) {
    return (chunk->light[idx3d(block_pos->xyz.x, block_pos->xyz.y,
                               block_pos->xyz.z, CHUNK_SIZE)] >>
            4) &
           0xf;
}

void chunk_set_skylight(chunk_t *chunk, const xvec3i64_t *block_pos,
                        uint8_t intensity) {
    size_t i = idx3d(block_pos->xyz.x, block_pos->xyz.y, block_pos->xyz.z,
                     CHUNK_SIZE);

    chunk->light[i] = (chunk->light[i] & 0xf) | (intensity << 4);
}

uint8_t chunk_get_block_light(const chunk_t *chunk,
                              const xvec3i64_t *block_pos) {
    return chunk->light[idx3d(block_pos->xyz.x, block_pos->xyz.y,
                              block_pos->xyz.z, CHUNK_SIZE)] &
           0xf;
}

void chunk_set_block_light(chunk_t *chunk, const xvec3i64_t *block_pos,
                           uint8_t intensity) {
    size_t i = idx3d(block_pos->xyz.x, block_pos->xyz.y, block_pos->xyz.z,
                     CHUNK_SIZE);

    chunk->light[i] = (chunk->light[i] & 0xf0) | intensity;
}