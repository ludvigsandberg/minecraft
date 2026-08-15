#include "chunk_map.h"

#include <stdlib.h>
#include <string.h>

static size_t hash_coord(const struct coord *c) {
    size_t h;

    /* Spatial hashing with 32-bit integer casting. */
    h = ((size_t)(unsigned int)c->x * 73856093UL) ^
        ((size_t)(unsigned int)c->y * 19349663UL) ^
        ((size_t)(unsigned int)c->z * 83492791UL);

    /* Bit-mixer (avalanche) to eliminate linear clustering on neighboring
       coordinates. */
    h ^= h >> 16;
    h *= 0x85ebca6bUL;
    h ^= h >> 13;

    return h;
}

#define COORD_EQ(a, b)                                                        \
    ((a)->x == (b)->x && (a)->y == (b)->y && (a)->z == (b)->z)

void chunk_map_init(struct chunk_map *map, size_t initial_cap) {
    size_t cap = 1;

    if (initial_cap < 16) {
        initial_cap = 16;
    }

    while (cap < initial_cap) {
        cap <<= 1;
    }

    map->capacity = cap;
    map->size     = 0;
    map->slots = (struct chunk_slot *)calloc(cap, sizeof(struct chunk_slot));
}

void chunk_map_free(struct chunk_map *map) {
    free(map->slots);
    map->slots    = NULL;
    map->capacity = 0;
    map->size     = 0;
}

struct chunk *chunk_map_get(const struct chunk_map *map,
                            const struct coord *key) {
    size_t hash;
    size_t mask;
    size_t idx;

    if (map->size == 0 || key == NULL) {
        return NULL;
    }

    hash = hash_coord(key);
    mask = map->capacity - 1;
    idx  = hash & mask;

    while (1) {
        struct chunk_slot *slot = &map->slots[idx];

        if (slot->state == SLOT_EMPTY) {
            return NULL;
        }

        if (slot->state == SLOT_OCCUPIED && slot->hash == hash) {
            if (COORD_EQ(&slot->key, key)) {
                return slot->value;
            }
        }

        idx = (idx + 1) & mask;
    }
}

static void chunk_map_grow(struct chunk_map *map) {
    size_t old_cap;
    size_t new_cap;
    size_t new_mask;
    size_t i;
    size_t idx;
    struct chunk_slot *old_slots;
    struct chunk_slot *new_slots;

    old_cap   = map->capacity;
    old_slots = map->slots;

    new_cap  = old_cap * 2;
    new_mask = new_cap - 1;

    new_slots =
        (struct chunk_slot *)calloc(new_cap, sizeof(struct chunk_slot));

    for (i = 0; i < old_cap; i++) {
        if (old_slots[i].state == SLOT_OCCUPIED) {
            idx = old_slots[i].hash & new_mask;

            while (new_slots[idx].state == SLOT_OCCUPIED) {
                idx = (idx + 1) & new_mask;
            }

            new_slots[idx] = old_slots[i];
        }
    }

    free(old_slots);
    map->slots    = new_slots;
    map->capacity = new_cap;
}

void chunk_map_insert(struct chunk_map *map, const struct coord *key,
                      struct chunk *val) {
    size_t hash;
    size_t mask;
    size_t idx;
    size_t first_deleted;

    if (key == NULL) {
        return 0;
    }

    if (map->size * 10 >= map->capacity * 7) {
        chunk_map_grow(map);
    }

    hash          = hash_coord(key);
    mask          = map->capacity - 1;
    idx           = hash & mask;
    first_deleted = (size_t)-1;

    while (1) {
        struct chunk_slot *slot = &map->slots[idx];

        if (slot->state == SLOT_EMPTY) {
            if (first_deleted != (size_t)-1) {
                idx = first_deleted;
            }
            break;
        }

        if (slot->state == SLOT_DELETED) {
            if (first_deleted == (size_t)-1) {
                first_deleted = idx;
            }
        } else if (slot->hash == hash && COORD_EQ(&slot->key, key)) {
            slot->value = val;
            return 0; /* Updated existing key. */
        }

        idx = (idx + 1) & mask;
    }

    map->slots[idx].key   = *key;
    map->slots[idx].hash  = hash;
    map->slots[idx].value = val;
    map->slots[idx].state = SLOT_OCCUPIED;
    map->size++;
    return 1; /* Inserted new key. */
}

void chunk_map_remove(struct chunk_map *map, const struct coord *key) {
    size_t hash;
    size_t mask;
    size_t idx;

    if (map->size == 0 || key == NULL) {
        return;
    }

    hash = hash_coord(key);
    mask = map->capacity - 1;
    idx  = hash & mask;

    while (1) {
        struct chunk_slot *slot = &map->slots[idx];

        if (slot->state == SLOT_EMPTY) {
            return;
        }

        if (slot->state == SLOT_OCCUPIED && slot->hash == hash &&
            COORD_EQ(&slot->key, key)) {
            slot->state = SLOT_DELETED;
            map->size--;
            return;
        }

        idx = (idx + 1) & mask;
    }
}
