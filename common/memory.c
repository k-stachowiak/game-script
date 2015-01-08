/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "log.h"
#include "memory.h"
#include "common.h"

#if MEM_TRACE

static struct MemNode {
    void *address;
    size_t size;
    struct MemNode *next;
} *mem_nodes;

static void mem_store(void *address, size_t size)
{
    struct MemNode *new_node = malloc(sizeof(*mem_nodes));
    new_node->address = address;
    new_node->size = size;
    new_node->next = mem_nodes;
    mem_nodes = new_node;
}

static void mem_discard(void *address)
{
    struct MemNode *curr, *next;

    if (!mem_nodes || (!mem_nodes->next && mem_nodes->address != address)) {
        goto fail;
    }

    curr = mem_nodes;
    next = mem_nodes->next;

    while (next) {
        if (next->address == address) {
            curr->next = next->next;
            free(next);
            return;
        } else {
            curr = next;
            next = next->next;
        }
    }

fail:
    LOG_ERROR("Memory manager corruption.");
    exit(1);
}

void mem_init(void)
{
    mem_nodes = NULL;
}

void mem_deinit(void)
{
    if (mem_nodes) {
        LOG_ERROR("Memory leaked.");
        while (mem_nodes) {
            struct MemNode *next = mem_nodes->next;
            free(mem_nodes->address);
            mem_nodes = next;
        }
    }
}

#endif

void *mem_malloc(size_t size)
{
	void *result = malloc(size);
	if (!result) {
		LOG_ERROR("Allocation failure.");
		exit(1);
	}

#if MEM_TRACE
    mem_store(result, size);
#endif

	return result;
}

void *mem_calloc(size_t count, size_t size)
{
	void *result = calloc(count, size);
	if (!result) {
		LOG_ERROR("Callocation failure.");
		exit(1);
	}

#if MEM_TRACE
    mem_store(result, size);
#endif

	return result;
}

void *mem_realloc(void *old, size_t size)
{
	void *result = realloc(old, size);
	if (!result) {
		LOG_ERROR("Reallocation failure.");
		exit(1);
	}

#if MEM_TRACE
    mem_discard(old);
    mem_store(result, size);
#endif

	return result;
}

void mem_free(void *ptr)
{
	free(ptr);

#if MEM_TRACE
    mem_discard(ptr);
#endif
}
