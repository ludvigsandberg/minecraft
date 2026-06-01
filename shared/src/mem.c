#include "shared/mem.h"

#include <stdlib.h>
#include <stdio.h>

#define REASONABLE_MAX_ALLOC 10000000

void *safe_malloc(size_t size) {
    void *p;

    assert(size < REASONABLE_MAX_ALLOC);

    p = malloc(size);

    if (p == NULL) {
        fprintf(stderr, "Out of memory. Requested %lu bytes.\n",
                (unsigned long)size);
        fflush(stderr);
        abort();
    }

    return p;
}

void *safe_calloc(size_t n, size_t size) {
    void *p;

    assert((n * size) < REASONABLE_MAX_ALLOC);

    p = calloc(n, size);

    if (p == NULL) {
        fprintf(stderr, "Out of memory. Requested %lu bytes.\n",
                (unsigned long)size);
        fflush(stderr);
        abort();
    }

    return p;
}

void *safe_realloc(void *ptr, size_t size) {
    void *p;

    assert(size < REASONABLE_MAX_ALLOC);

    p = realloc(ptr, size);

    if (p == NULL) {
        fprintf(stderr, "Out of memory. Requested %lu bytes.\n",
                (unsigned long)size);
        fflush(stderr);
        abort();
    }

    return p;
}
