#ifndef COMMON_MEM_H
#define COMMON_MEM_H

#include <stddef.h>

void *checked_malloc(size_t size);
void *checked_calloc(size_t n, size_t size);
void *checked_realloc(void *ptr, size_t size);

#endif
