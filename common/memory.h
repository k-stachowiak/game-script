/* Copyright (C) 2015 Krzysztof Stachowiak */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#define MEM_TRACE 1

#if MEM_TRACE
void mem_init(void);
void mem_deinit(void);
#endif

void *mem_malloc(size_t size);
void *mem_calloc(size_t count, size_t size);
void *mem_realloc(void *old, size_t size);
void mem_free(void *ptr);

#endif
