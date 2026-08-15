#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

enum map_slot { SLOT_EMPTY, SLOT_OCCUPIED, SLOT_DELETED };

/* Flat hash-map with linear probing. */
struct map {
    size_t size;
    size_t capacity; /* Capacity must be a power of two! */
};

#define MAP_FIND_SLOT(BUCKETS, MAP, HASHED_KEY, KEY_PTR, CMP_FN, OUT_IDX,     \
                      FOUND_FLAG)                                             \
    do {                                                                      \
        size_t _cap           = (MAP).capacity;                               \
        size_t _mask          = _cap - 1;                                     \
        size_t _idx           = (HASHED_KEY) & _mask;                         \
        size_t _first_deleted = (size_t)-1;                                   \
        (FOUND_FLAG)          = 0;                                            \
        (OUT_IDX)             = (size_t)-1;                                   \
        assert((_cap & (_cap - 1)) == 0);                                     \
        assert(_cap > 0);                                                     \
                                                                              \
        while (1) {                                                           \
            if ((BUCKETS)[_idx].slot == SLOT_EMPTY) {                         \
                if ((OUT_IDX) == (size_t)-1) {                                \
                    (OUT_IDX) = _idx;                                         \
                }                                                             \
                break;                                                        \
            } else if ((BUCKETS)[_idx].slot == SLOT_DELETED) {                \
                if (_first_deleted == (size_t)-1) {                           \
                    _first_deleted = _idx;                                    \
                }                                                             \
            } else if ((BUCKETS)[_idx].slot == SLOT_OCCUPIED) {               \
                if (CMP_FN(&(BUCKETS)[_idx].key, (KEY_PTR))) {                \
                    (OUT_IDX)    = _idx;                                      \
                    (FOUND_FLAG) = 1;                                         \
                    break;                                                    \
                }                                                             \
            }                                                                 \
            _idx = (_idx + 1) & _mask;                                        \
        }                                                                     \
        if (!(FOUND_FLAG) && _first_deleted != (size_t)-1) {                  \
            (OUT_IDX) = _first_deleted;                                       \
        }                                                                     \
    } while (0)

/* Check if a key exists in the map. Sets OUT_EXISTS to 1 or 0. */
#define MAP_CONTAINS(BUCKETS, MAP, KEY, CMP_FN, HASH_FN, OUT_EXISTS)          \
    do {                                                                      \
        (OUT_EXISTS) = 0;                                                     \
        if ((MAP).size > 0 && (BUCKETS) != NULL) {                            \
            size_t _h = HASH_FN(&(KEY));                                      \
            size_t _idx;                                                      \
            int _found;                                                       \
            MAP_FIND_SLOT(BUCKETS, MAP, _h, &(KEY), CMP_FN, _idx, _found);    \
            if (_found) {                                                     \
                (OUT_EXISTS) = 1;                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

/* Retrieve a value from the map. Sets OUT_FOUND to 1 if found, 0 otherwise. */
#define MAP_GET(BUCKETS, MAP, KEY, CMP_FN, HASH_FN, OUT_VALUE, OUT_FOUND)     \
    do {                                                                      \
        (OUT_FOUND) = 0;                                                      \
        if ((MAP).size > 0 && (BUCKETS) != NULL) {                            \
            size_t _h = HASH_FN(&(KEY));                                      \
            size_t _idx;                                                      \
            int _found;                                                       \
            MAP_FIND_SLOT(BUCKETS, MAP, _h, &(KEY), CMP_FN, _idx, _found);    \
            if (_found) {                                                     \
                (OUT_VALUE) = (BUCKETS)[_idx].value;                          \
                (OUT_FOUND) = 1;                                              \
            }                                                                 \
        }                                                                     \
    } while (0)

/* Insert or update a key/value pair. Handles automatic re-hashing and
   expansion. */
#define MAP_INSERT(BUCKETS, MAP, KEY, VALUE, CMP_FN, HASH_FN)                 \
    do {                                                                      \
        /* Grow at 70% load factor */                                         \
        if ((MAP).size * 10 >= (MAP).capacity * 7 || (BUCKETS) == NULL) {     \
            size_t _old_cap    = (MAP).capacity;                              \
            size_t _new_cap    = (_old_cap == 0) ? 16 : (_old_cap * 2);       \
            void *_old_buckets = (BUCKETS);                                   \
            size_t _b_size     = _new_cap * sizeof(*(BUCKETS));               \
            void *_new_buckets = malloc(_b_size);                             \
            if (!_new_buckets) {                                              \
                fprintf(stderr, "%s:%d Out of memory!\n", __FILE__,           \
                        __LINE__);                                            \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
            memset(_new_buckets, 0, _b_size);                                 \
            (BUCKETS)      = _new_buckets;                                    \
            (MAP).capacity = _new_cap;                                        \
            (MAP).size     = 0;                                               \
                                                                              \
            /* Re-hash existing occupied slots into new bucket array. */      \
            if (_old_buckets) {                                               \
                size_t _i;                                                    \
                for (_i = 0; _i < _old_cap; _i++) {                           \
                    if (((struct chunk_entry *)_old_buckets)[_i].slot ==      \
                        SLOT_OCCUPIED) {                                      \
                        size_t _h = HASH_FN(                                  \
                            &((struct chunk_entry *)_old_buckets)[_i].key);   \
                        size_t _dummy_idx;                                    \
                        int _dummy_found;                                     \
                        MAP_FIND_SLOT(                                        \
                            (BUCKETS), (MAP), _h,                             \
                            &((struct chunk_entry *)_old_buckets)[_i].key,    \
                            CMP_FN, _dummy_idx, _dummy_found);                \
                        (BUCKETS)[_dummy_idx].key =                           \
                            ((struct chunk_entry *)_old_buckets)[_i].key;     \
                        (BUCKETS)[_dummy_idx].value =                         \
                            ((struct chunk_entry *)_old_buckets)[_i].value;   \
                        (BUCKETS)[_dummy_idx].slot = SLOT_OCCUPIED;           \
                        (MAP).size++;                                         \
                    }                                                         \
                }                                                             \
                free(_old_buckets);                                           \
            }                                                                 \
        }                                                                     \
                                                                              \
        {                                                                     \
            size_t _h = HASH_FN(&(KEY));                                      \
            size_t _idx;                                                      \
            int _found;                                                       \
            MAP_FIND_SLOT(BUCKETS, MAP, _h, &(KEY), CMP_FN, _idx, _found);    \
            (BUCKETS)[_idx].key   = (KEY);                                    \
            (BUCKETS)[_idx].value = (VALUE);                                  \
            (BUCKETS)[_idx].slot  = SLOT_OCCUPIED;                            \
            if (!_found) {                                                    \
                (MAP).size++;                                                 \
            }                                                                 \
        }                                                                     \
    } while (0)

/* Remove an element from the map by setting its slot to SLOT_DELETED. */
#define MAP_REMOVE(BUCKETS, MAP, KEY, CMP_FN, HASH_FN, OUT_REMOVED)           \
    do {                                                                      \
        (OUT_REMOVED) = 0;                                                    \
        if ((MAP).size > 0 && (BUCKETS) != NULL) {                            \
            size_t _h = HASH_FN(&(KEY));                                      \
            size_t _idx;                                                      \
            int _found;                                                       \
            MAP_FIND_SLOT(BUCKETS, MAP, _h, &(KEY), CMP_FN, _idx, _found);    \
            if (_found) {                                                     \
                (BUCKETS)[_idx].slot = SLOT_DELETED;                          \
                (MAP).size--;                                                 \
                (OUT_REMOVED) = 1;                                            \
            }                                                                 \
        }                                                                     \
    } while (0)

#endif