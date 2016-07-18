/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *mem_malloc(size_t size);
void *mem_calloc(size_t count, size_t size);
void *mem_realloc(void *old, size_t size);
void mem_free(void *ptr);

#endif
