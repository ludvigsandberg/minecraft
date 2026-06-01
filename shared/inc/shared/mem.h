#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stddef.h>

void *safe_malloc(size_t size);
void *safe_calloc(size_t n, size_t size);
void *safe_realloc(void *ptr, size_t size);

#endif
