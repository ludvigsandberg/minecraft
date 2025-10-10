#include <world.h>

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <chunk.h>

void generate(blocks_t blocks, coord_t chunk_coord) {
    for (size_t x = 0; x < CHUNK_SIZE; x++) {
        for (size_t y = 0; y < CHUNK_SIZE; y++) {
            for (size_t z = 0; z < CHUNK_SIZE; z++) {
                uint8_t *block = &blocks[z * (CHUNK_SIZE * CHUNK_SIZE) +
                                         y * CHUNK_SIZE + x];

                coord_t world_coord = {chunk_coord[0] * CHUNK_SIZE + x,
                                       chunk_coord[1] * CHUNK_SIZE + y,
                                       chunk_coord[2] * CHUNK_SIZE + z};

                float noise_x =
                    (float)(chunk_coord[0] * CHUNK_SIZE + x) / 20.f;
                float noise_y =
                    (float)(chunk_coord[1] * CHUNK_SIZE + y) / 20.f;
                float noise_z =
                    (float)(chunk_coord[2] * CHUNK_SIZE + z) / 20.f;

                float n =
                    stb_perlin_noise3(noise_x, noise_y, noise_z, 0, 0, 0);

                if (n > 0.6f) {
                    *block = BLOCK_STONE;
                } else if (n > 0.55f) {
                    *block = BLOCK_GRASS;
                } else {
                    *block = BLOCK_AIR;
                }
            }
        }
    }
}

static void world_to_loaded_chunk_coord(const world_t *world,
                                        const coord_t world_chunk_coord,
                                        coord_t loaded_chunk_coord) {
    for (size_t i = 0; i < 3; i++) {
        loaded_chunk_coord[i] = world_chunk_coord[i] -
                                world->chunk_center_coord[i] + RENDER_DISTANCE;
    }
}

size_t world_chunk_coord_to_index(const world_t *world,
                                  const coord_t chunk_coord) {
    coord_t local_chunk_coord;
    world_to_loaded_chunk_coord(world, chunk_coord, local_chunk_coord);

    return local_chunk_coord[0] +
           (2 * RENDER_DISTANCE + 1) *
               (local_chunk_coord[1] +
                (2 * RENDER_DISTANCE + 1) * local_chunk_coord[2]);
}

void world_new(world_t *world) {
    for (size_t i = 0; i < 3; i++) {
        world->chunk_center_coord[i] = 0;
    }

    coord_t chunk_coord;

    for (chunk_coord[0] = -RENDER_DISTANCE; chunk_coord[0] <= RENDER_DISTANCE;
         chunk_coord[0]++) {
        for (chunk_coord[1] = -RENDER_DISTANCE;
             chunk_coord[1] <= RENDER_DISTANCE; chunk_coord[1]++) {
            for (chunk_coord[2] = -RENDER_DISTANCE;
                 chunk_coord[2] <= RENDER_DISTANCE; chunk_coord[2]++) {
                chunk_t *chunk = malloc(sizeof(chunk_t));
                assert(chunk);
                blocks_t blocks;
                generate(blocks, chunk_coord);
                chunk_new(chunk, blocks);

                world->loaded_chunks[world_chunk_coord_to_index(
                    world, chunk_coord)] = chunk;
            }
        }
    }
}

void world_free(world_t *world) {
    for (size_t i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        free(world->loaded_chunks[i]);
    }
}

void world_update(world_t *world) {
}
