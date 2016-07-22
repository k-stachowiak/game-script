/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>

#include "log.h"
#include "memory.h"

void *mem_malloc(size_t size)
{
    void *result = malloc(size);
    if (!result) {
        LOG_ERROR("Allocation failure.");
        exit(1);
    }

    return result;
}

void *mem_calloc(size_t count, size_t size)
{
    void *result = calloc(count, size);
    if (!result) {
        LOG_ERROR("Callocation failure.");
        exit(1);
    }

    return result;
}

void *mem_realloc(void *old, size_t size)
{
    void *result = realloc(old, size);
    if (!result) {
        LOG_ERROR("Reallocation failure.");
        exit(1);
    }

    return result;
}

void mem_free(void *ptr)
{
    free(ptr);
}
