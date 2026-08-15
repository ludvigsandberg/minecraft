#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include <stddef.h>

#include "coord.h"

#define SLOT_EMPTY    0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED  2

/* 32 bytes. */
struct chunk_slot {
    struct coord key;
    unsigned int state;
    size_t hash;
    struct chunk *value;
};

/* Flat hash-map of coordinates to pointer-to-chunks with linear probing. */
struct chunk_map {
    struct chunk_slot *slots;
    size_t capacity; /* Always a power of two. */
    size_t size;
};

void chunk_map_init(struct chunk_map *map, size_t initial_cap);
void chunk_map_free(struct chunk_map *map);

struct chunk *chunk_map_get(const struct chunk_map *map,
                            const struct coord *key);
void chunk_map_insert(struct chunk_map *map, const struct coord *key,
                      struct chunk *val);
void chunk_map_remove(struct chunk_map *map, const struct coord *key);

#endif