#include "client/world.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "macros.h"
#include "array.h"
#include "client/chunk.h"
#include "client/renderer.h"

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

    if (x < 0)
        xi--;
    if (y < 0)
        yi--;
    if (z < 0)
        zi--;

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

static void generate(unsigned char *blocks, const chunk_coord_t *coord) {
    long x;
    long y;
    long z;

    for (x = 0; x < CHUNK_SIZE; x++) {
        for (y = 0; y < CHUNK_SIZE; y++) {
            for (z = 0; z < CHUNK_SIZE; z++) {
                unsigned char *block;
                coord_t block_coord;
                float noise_x;
                float noise_y;
                float noise_z;
                float n;

                block = &blocks[INDEX_3D(x, y, z, CHUNK_SIZE)];

                block_coord.x = coord->x * CHUNK_SIZE + x;
                block_coord.y = coord->y * CHUNK_SIZE + y;
                block_coord.z = coord->z * CHUNK_SIZE + z;

                noise_x = (float)(coord->x * CHUNK_SIZE + x) / 25.0f;
                noise_y = (float)(coord->y * CHUNK_SIZE + y) / 25.0f;
                noise_z = (float)(coord->z * CHUNK_SIZE + z) / 25.0f;

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
}

static void world_to_local_chunk_coord(const chunk_coord_t *coord,
                                       const chunk_coord_t *center,
                                       chunk_coord_t *out_local) {
    out_local->x = coord->x - center->x + RENDER_DISTANCE;
    out_local->y = coord->y - center->y + RENDER_DISTANCE;
    out_local->z = coord->z - center->z + RENDER_DISTANCE;
}

static size_t local_chunk_coord_to_index(const chunk_coord_t *local) {
    return (size_t)INDEX_3D(local->x, local->y, local->z, LOADED_CHUNKS_LEN);
}

static size_t chunk_coord_to_index(const chunk_coord_t *coord,
                                   const chunk_coord_t *center) {
    chunk_coord_t local;
    world_to_local_chunk_coord(coord, center, &local);

    return local_chunk_coord_to_index(&local);
}

static int world_is_chunk_loaded(const world_t *world,
                                 const chunk_coord_t *coord,
                                 chunk_t **out_chunk) {
    chunk_t *chunk_maybe_loaded;

    assert(world);
    assert(coord);
    assert(out_chunk);

    if (coord->x < world->center.x - RENDER_DISTANCE ||
        coord->x > world->center.x + RENDER_DISTANCE ||

        coord->y < world->center.y - RENDER_DISTANCE ||
        coord->y > world->center.y + RENDER_DISTANCE ||

        coord->z < world->center.z - RENDER_DISTANCE ||
        coord->z > world->center.z + RENDER_DISTANCE) {
        return FALSE;
    }

    chunk_maybe_loaded =
        world->loaded_chunks[chunk_coord_to_index(coord, &world->center)];

    if (!chunk_maybe_loaded) {
        return FALSE;
    }

    *out_chunk = chunk_maybe_loaded;

    return TRUE;
}

static void *chunk_load_thread(void *ctx) {
    world_t *world = ctx;

    while (TRUE) {
        chunk_job_t *job = NULL;
        chunk_result_t *res;

        pthread_mutex_lock(&world->mutex);

        while (world->is_running && world->job_queue_count == 0) {
            pthread_cond_wait(&world->cond, &world->mutex);
        }

        if (!world->is_running) {
            pthread_mutex_unlock(&world->mutex);
            break;
        }

        if (world->job_queue_count > 0) {
            job = world->job_queue[world->job_queue_count - 1];

            world->job_queue_count--;
        }

        pthread_mutex_unlock(&world->mutex);

        res        = malloc(sizeof(chunk_result_t));
        res->coord = job->coord;
        generate(res->blocks, &res->coord);

        free(job);

        pthread_mutex_lock(&world->mutex);
        ARR_APPEND(world->result_queue, world->result_queue_count,
                   world->result_queue_cap, &res);
        pthread_mutex_unlock(&world->mutex);
    }

    return NULL;
}

static void load_chunk(world_t *world, const chunk_coord_t *coord) {
    chunk_job_t *job = malloc(sizeof(chunk_job_t));
    job->coord       = *coord;

    pthread_mutex_lock(&world->mutex);

    ARR_APPEND(world->job_queue, world->job_queue_count, world->job_queue_cap,
               &job);
    pthread_cond_signal(&world->cond);
    pthread_mutex_unlock(&world->mutex);
}

void world_init(world_t *world) {
    chunk_coord_t coord;

    /* Setup chunk loading threads. */

    world->job_queue          = NULL;
    world->job_queue_count    = 0;
    world->job_queue_cap      = 0;
    world->result_queue       = NULL;
    world->result_queue_count = 0;
    world->result_queue_cap   = 0;

    pthread_mutex_init(&world->mutex, NULL);
    pthread_cond_init(&world->cond, NULL);
    world->is_running = TRUE;
    pthread_create(&world->thread, NULL, chunk_load_thread, world);

    /* Load chunks. */

    world->center.x = 0;
    world->center.y = 0;
    world->center.z = 0;

    memset(world->loaded_chunks, 0, LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

    for (coord.x = -RENDER_DISTANCE; coord.x <= RENDER_DISTANCE; coord.x++) {
        for (coord.y = -RENDER_DISTANCE; coord.y <= RENDER_DISTANCE;
             coord.y++) {
            for (coord.z = -RENDER_DISTANCE; coord.z <= RENDER_DISTANCE;
                 coord.z++) {
                load_chunk(world, &coord);
            }
        }
    }
}

void world_update(world_t *world, const camera_t *cam) {
    chunk_coord_t camera_coord;
    size_t i;
    int is_camera_in_different_chunk = FALSE;

    camera_coord.x = (long)cam->pos.VEC_X / CHUNK_SIZE;
    camera_coord.y = (long)cam->pos.VEC_Y / CHUNK_SIZE;
    camera_coord.z = (long)cam->pos.VEC_Z / CHUNK_SIZE;

    /* Poll threads for new chunks. */

    pthread_mutex_lock(&world->mutex);

    for (i = 0; i < world->result_queue_count; i++) {
        chunk_result_t *result;
        int is_within_region = TRUE;

        result = world->result_queue[i];

        if (labs(result->coord.x - camera_coord.x) > RENDER_DISTANCE ||
            labs(result->coord.y - camera_coord.y) > RENDER_DISTANCE ||
            labs(result->coord.z - camera_coord.z) > RENDER_DISTANCE) {
            is_within_region = FALSE;
        }

        if (is_within_region) {
            chunk_t *chunk = malloc(sizeof(chunk_t));
            chunk_init(chunk, result->blocks, &result->coord);

            world->loaded_chunks[chunk_coord_to_index(&result->coord,
                                                      &world->center)] = chunk;
        }

        free(result);
    }

    world->result_queue_count = 0;

    pthread_mutex_unlock(&world->mutex);

    /* Check if camera has moved to a different chunk. */
    if (world->center.x != camera_coord.x ||
        world->center.y != camera_coord.y ||
        world->center.z != camera_coord.z) {
        is_camera_in_different_chunk = TRUE;
    }

    /* Move and generate chunks. */
    if (is_camera_in_different_chunk) {
        chunk_coord_t old_center;
        chunk_coord_t diff;
        chunk_t *old_loaded_chunks[LOADED_CHUNKS_TOTAL];
        long x;
        long y;
        long z;

        old_center    = world->center;
        world->center = camera_coord;

        diff.x = world->center.x - old_center.x;
        diff.y = world->center.y - old_center.y;
        diff.z = world->center.z - old_center.z;

        memcpy(old_loaded_chunks, world->loaded_chunks,
               LOADED_CHUNKS_TOTAL * sizeof(chunk_t *));

        /* For each chunk in the new region, check if we can copy the chunk
           from the previously loaded region or if a new one has to be
           generated. */

        for (x = 0; x < LOADED_CHUNKS_LEN; x++) {
            for (y = 0; y < LOADED_CHUNKS_LEN; y++) {
                for (z = 0; z < LOADED_CHUNKS_LEN; z++) {
                    chunk_coord_t coord;
                    chunk_coord_t old_coord;
                    int delete   = FALSE;
                    int can_copy = TRUE;

                    coord.x = x;
                    coord.y = y;
                    coord.z = z;

                    old_coord.x = x + diff.x;
                    old_coord.y = y + diff.y;
                    old_coord.z = z + diff.z;

                    /* Free chunk if it has moved out of new area. */

                    if (coord.x < diff.x ||
                        coord.x >= diff.x + LOADED_CHUNKS_LEN ||
                        coord.y < diff.y ||
                        coord.y >= diff.y + LOADED_CHUNKS_LEN ||
                        coord.z < diff.z ||
                        coord.z >= diff.z + LOADED_CHUNKS_LEN) {
                        delete = TRUE;
                    }

                    if (delete) {
                        size_t idx = local_chunk_coord_to_index(&coord);

                        if (old_loaded_chunks[idx]) {
                            chunk_coord_t below_coord;
                            chunk_t *below_chunk;

                            /* Set chunk below to dirty. */

                            below_coord = old_loaded_chunks[idx]->coord;
                            below_coord.y--;

                            if (world_is_chunk_loaded(world, &below_coord,
                                                      &below_chunk)) {
                                below_chunk->dirty = TRUE;
                            }

                            /* Free. */
                            chunk_free(old_loaded_chunks[idx]);
                            free(old_loaded_chunks[idx]);
                            old_loaded_chunks[idx] = NULL;
                        }
                    }

                    /* Copy chunk if it is still within area. */

                    if (old_coord.x < 0 || old_coord.x >= LOADED_CHUNKS_LEN ||
                        old_coord.y < 0 || old_coord.y >= LOADED_CHUNKS_LEN ||
                        old_coord.z < 0 || old_coord.z >= LOADED_CHUNKS_LEN) {
                        can_copy = FALSE;
                    }

                    /* Copy chunk. */
                    if (can_copy) {
                        chunk_t *old =
                            old_loaded_chunks[local_chunk_coord_to_index(
                                &old_coord)];

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &coord)] = old;
                    }
                    /* Generate new chunk. */
                    else {
                        chunk_coord_t chunk_coord;

                        chunk_coord.x =
                            coord.x + world->center.x - RENDER_DISTANCE;
                        chunk_coord.y =
                            coord.y + world->center.y - RENDER_DISTANCE;
                        chunk_coord.z =
                            coord.z + world->center.z - RENDER_DISTANCE;

                        world->loaded_chunks[local_chunk_coord_to_index(
                            &coord)] = NULL;

                        load_chunk(world, &chunk_coord);
                    }
                }
            }
        }
    }

    /* Update chunks. */
    for (i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->loaded_chunks[i]) {
            chunk_update(world->loaded_chunks[i], world);
        }
    }
}

void world_draw(world_t *world, const renderer_t *renderer, camera_t *camera) {
    int i;

    for (i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->loaded_chunks[i]) {
            renderer_draw_chunk(renderer, world->loaded_chunks[i], camera);
        }
    }
}
