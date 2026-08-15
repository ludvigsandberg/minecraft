#include "client/world.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "macros.h"

static struct chunk *get_chunk(struct world *world,
                               const struct coord *coord) {
    assert(world);
    assert(coord);

    /* TODO: Return chunk from map. */

    return NULL;
}

static float noise3d(float x, float y, float z) {
    int xi, yi, zi;
    int i;
    float xf, yf, zf;
    float c[2][2][2];
    float u, v, w;
    float a, b, c1, d;

    xi = (int)x;
    yi = (int)y;
    zi = (int)z;

    if (x < 0) {
        xi--;
    }
    if (y < 0) {
        yi--;
    }
    if (z < 0) {
        zi--;
    }

    xf = x - (float)xi;
    yf = y - (float)yi;
    zf = z - (float)zi;

    u = xf * xf * (3.0f - 2.0f * xf);
    v = yf * yf * (3.0f - 2.0f * yf);
    w = zf * zf * (3.0f - 2.0f * zf);

    for (i = 0; i < 8; i++) {
        int xx = xi + (i & 1);
        int yy = yi + ((i >> 1) & 1);
        int zz = zi + ((i >> 2) & 1);
        int n;

        n = xx * 15731 + yy * 789221 + zz * 1376312589;
        n = (n << 13) ^ n;

        c[i & 1][(i >> 1) & 1][(i >> 2) & 1] =
            1.0f - ((float)((n * (n * n * 15731 + 789221) + 1376312589) &
                            0x7fffffff) /
                    1073741824.0f);
    }

    a  = c[0][0][0] * (1.0f - u) + c[1][0][0] * u;
    b  = c[0][1][0] * (1.0f - u) + c[1][1][0] * u;
    c1 = c[0][0][1] * (1.0f - u) + c[1][0][1] * u;
    d  = c[0][1][1] * (1.0f - u) + c[1][1][1] * u;

    a = a * (1.0f - v) + b * v;
    b = c1 * (1.0f - v) + d * v;

    return a * (1.0f - w) + b * w;
}

static void generate(void *context) {
    const struct world *world;
    struct coord coord;

    struct gen_result *result;

    int x;
    int y;
    int z;

    assert(context);

    world = ((struct gen_context *)context)->world;
    coord = ((struct gen_context *)context)->coord;

    /* The chunk might have been unloaded while this task was enqueued. */
    pthread_mutex_lock(&world->mutex);
    if (!get_chunk(&world, &coord)) {
        pthread_mutex_unlock(&world->mutex);
        return;
    }
    pthread_mutex_unlock(&world->mutex);

    result = malloc(sizeof(struct generation_result));
    if (!result) {
        printf("%s:%d Out of memory!\r\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    result->coord = coord;

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                unsigned char *block;
                struct coord block_coord;
                float noise_x;
                float noise_y;
                float noise_z;
                float n;

                block = &result->blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];

                block_coord.x = coord.x * CHUNK_SIZE + x;
                block_coord.y = coord.y * CHUNK_SIZE + y;
                block_coord.z = coord.z * CHUNK_SIZE + z;

                noise_x = (float)(coord.x * CHUNK_SIZE + x) / 25.0f;
                noise_y = (float)(coord.y * CHUNK_SIZE + y) / 25.0f;
                noise_z = (float)(coord.z * CHUNK_SIZE + z) / 25.0f;

                n = noise3d(noise_x, noise_y, noise_z);

                if (block_coord.y < -10) {
                    if (n > 0.2f) {
                        *block = BLOCK_AIR;
                    } else {
                        *block = BLOCK_BEDROCK;
                    }
                } else {
                    if (n > 0.28f) {
                        *block = (n > 0.29f) ? BLOCK_STONE : BLOCK_GRASS;
                    } else {
                        *block = BLOCK_AIR;
                    }
                }
            }
        }
    }

    /* Push result onto queue. */
    pthread_mutex_lock(&world->mutex);
    QUEUE_PUSH(world->gens_elems, world->gens, &result);
    pthread_mutex_unlock(&world->mutex);

    free(context);
}

static const struct coord neighbor_offsets[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

static void mesh(void *void_context) {
    struct mesh_context *context;
    struct mesh_result *result;

    int i;
    int x;
    int y;
    int z;

    struct chunk *chunk;
    struct coord neighbor_coords[6];

    /* Chunk blocks padded with adjacent blocks from neighboring chunks. */
    unsigned char
        blocks[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)] = {0};

    assert(void_context);

    context = (struct mesh_context *)void_context;

    for (i = 0; i < 6; i++) {
        neighbor_coords[i] = coord_add(&context->coord, &neighbor_offsets[i]);
    }

    /* Populate block array. */

    pthread_mutex_lock(&context->world->mutex);

    chunk = get_chunk(&context->world, &context->coord);
    if (!chunk) {
        pthread_mutex_unlock(&context->world->mutex);
        free(context);
        return;
    }
    for (y = 0; y < CHUNK_SIZE; y++) {
        for (z = 0; z < CHUNK_SIZE; z++) {
            for (x = 0; x < CHUNK_SIZE; x++) {
                blocks[INDEX_3D(x + 1, y + 1, z + 1, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[0]);
    if (chunk) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                blocks[INDEX_3D(17, y + 1, z + 1, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(0, y, z, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[1]);
    if (chunk) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                blocks[INDEX_3D(0, y + 1, z + 1, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(CHUNK_SIZE - 1, y, z, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[2]);
    if (chunk) {
        for (z = 0; z < CHUNK_SIZE; z++) {
            for (x = 0; x < CHUNK_SIZE; x++) {
                blocks[INDEX_3D(x + 1, 17, z + 1, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(x, 0, z, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[3]);
    if (chunk) {
        for (z = 0; z < CHUNK_SIZE; z++) {
            for (x = 0; x < CHUNK_SIZE; x++) {
                blocks[INDEX_3D(x + 1, 0, z + 1, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(x, CHUNK_SIZE - 1, z, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[4]);
    if (chunk) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (x = 0; x < CHUNK_SIZE; x++) {
                blocks[INDEX_3D(x + 1, y + 1, 17, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(x, y, 0, CHUNK_SIZE)];
            }
        }
    }
    chunk = get_chunk(context->world, neighbor_coords[5]);
    if (chunk) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (x = 0; x < CHUNK_SIZE; x++) {
                blocks[INDEX_3D(x + 1, y + 1, 0, CHUNK_SIZE + 2)] =
                    chunk->blocks[INDEX_3D(x, y, CHUNK_SIZE - 1, CHUNK_SIZE)];
            }
        }
    }

    pthread_mutex_unlock(&task->world->mutex);

    result = malloc(sizeof(struct meshing_result));
    if (!result) {
        printf("%s:%d Out of memory!\r\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    result->coord = task->coord;

    /* Construct chunk mesh here... */

    pthread_mutex_lock(&task->world->mutex);
    RING_BUFFER_PUSH(context->world->meshes, context->world->mesh_elems,
                     &result);
    pthread_mutex_unlock(&task->world->mutex);

    free(context);
}

void world_init(struct world *world) {
    assert(world);

    /* Setup thread pool. */

    pthread_mutex_init(&world->mutex, NULL);
    pthread_cond_init(&world->cond, NULL);
}

void world_update(struct world *world, const struct camera *camera) {
    struct coord camera_coord;
    int i;

    assert(world);
    assert(camera);

    camera_coord.x = (int)camera->pos.VEC_X / CHUNK_SIZE;
    camera_coord.y = (int)camera->pos.VEC_Y / CHUNK_SIZE;
    camera_coord.z = (int)camera->pos.VEC_Z / CHUNK_SIZE;

    /* Poll task results. */

    pthread_mutex_lock(&world->mutex);

    while (world->generation_results_info.size > 0) {
        struct generation_result *result;
        QUEUE_POP(world->generation_results, world->generation_results_info,
                  &result);

        /* Do something with the result... */

        free(result);
    }

    while (world->meshing_results_info.size > 0) {
        struct generation_result *result;
        QUEUE_POP(world->meshing_results, world->meshing_results_info,
                  &result);

        /* Do something with the result... */

        free(result);
    }

    pthread_mutex_unlock(&world->mutex);

    /* Update chunks. */
    for (i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->chunks[i]) {
            chunk_update(world->chunks[i], world);
        }
    }
}
