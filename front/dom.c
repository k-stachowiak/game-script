/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "dom.h"

static char *reserved[] = {
	"do",
	"bind",
	"if",
	"func",
	"true",
	"false"
};

static int reserved_count = sizeof(reserved) / sizeof(reserved[0]);

static int dom_list_length(struct DomNode* current)
{
    int result = 0;
    while (current) {
		current = current->next;
		++result;
    }
    return result;
}

struct DomNode *dom_make_atom(struct SourceLocation *loc, char *begin, char *end)
{
	struct DomNode *result = malloc_or_die(sizeof(*result));
    int length = end - begin;
    result->loc = *loc;
    result->type = DOM_ATOM;
	result->atom = malloc_or_die(length + 1);
    memcpy(result->atom, begin, length);
    result->atom[length] = '\0';
    result->next = NULL;
    return result;
}

struct DomNode *dom_make_compound(
        struct SourceLocation *loc,
        enum DomCpdType compound_type,
        struct DomNode *children)
{
	struct DomNode *result = malloc_or_die(sizeof(*result));
    result->loc = *loc;
    result->type = DOM_COMPOUND;
    result->cpd_type = compound_type;
    result->cpd_children = children;
    result->next = NULL;
    return result;
}

void dom_free(struct DomNode *current)
{
    while (current) {
        struct DomNode *next = current->next;
        if (current->type == DOM_ATOM) {
            free(current->atom);
        } else {
            dom_free(current->cpd_children);
        }
        free(current);
        current = next;
    }
}

bool dom_node_is_atom(struct DomNode *node)
{
    return node->type == DOM_ATOM;
}

bool dom_node_is_reserved_atom(struct DomNode *node, enum Reserved res)
{
    if (node->type != DOM_ATOM) {
        return false;
    }

    return (strcmp(node->atom, reserved[(size_t)res]) == 0);
}

bool dom_node_is_compound(struct DomNode *node)
{
    return node->type == DOM_COMPOUND;
}

bool dom_node_is_spec_compound(struct DomNode *node, enum DomCpdType type)
{
    return node->type == DOM_COMPOUND && node->cpd_type == type;
}

bool dom_node_is_cpd_of_size(struct DomNode *node, int size)
{
    return node->type == DOM_COMPOUND &&
       dom_list_length(node->cpd_children) == size;
}

bool dom_node_is_cpd_min_size(struct DomNode *node, int size)
{
    return node->type == DOM_COMPOUND &&
        dom_list_length(node->cpd_children) >= size;
}

char *dom_node_parse_symbol(struct DomNode *node)
{
    char* result;
    int len, i;

    if (node->type != DOM_ATOM) {
        return NULL;
    }

	for (i = 0; i < reserved_count; ++i) {
		if (strcmp(node->atom, reserved[i]) == 0) {
			return NULL;
		}
	}

    len = strlen(node->atom);
	result = malloc_or_die(len + 1);
	memcpy(result, node->atom, len + 1);
    result[len] = '\0';

    return result;
}

