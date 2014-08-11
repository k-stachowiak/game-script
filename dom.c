#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "dom.h"

struct DomNode *dom_make_atom(struct Location loc, char *begin, char *end)
{
    struct DomNode *result = malloc(sizeof(*result));
    int length = end - begin;

    if (!result) {
        err_set(ERR_DOM, "Allocation failed.");
        return NULL;
    }

    result->loc = loc;
    result->type = DOM_ATOM;
    result->atom = malloc(length + 1);
    if (!result->atom) {
        err_set(ERR_DOM, "Allocation failed.");
        return NULL;
    }

    memcpy(result->atom, begin, length);
    result->atom[length] = '\0';
    result->next = NULL;
    return result;
}

struct DomNode *dom_make_compound(
        struct Location loc,
        enum DomCpdType compound_type,
        struct DomNode *children)
{
    struct DomNode *result = malloc(sizeof(*result));
    if (!result) {
        err_set(ERR_DOM, "Allocation failed.");
        return NULL;
    }

    result->loc = loc;
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

bool dom_node_is_spec_atom(struct DomNode *node, char *atom)
{
    if (node->type != DOM_ATOM) {
        return false;
    }

    return (strcmp(node->atom, atom) == 0);
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

char *dom_node_parse_atom(struct DomNode *node)
{
    char* result;
    int len;
    if (node->type == DOM_COMPOUND) {
        return NULL;
    }

    len = strlen(node->atom);
    result = malloc(len + 1);
    if (!result) {
        err_set(ERR_DOM, "Allocation failed.");
        return NULL;
    }

    strcpy(result, node->atom);
    result[len] = '\0';

    return result;
}

void dom_visit(struct DomNode *dom, void (*f)(struct DomNode*))
{
    f(dom);
    if (dom->type == DOM_COMPOUND) {
        struct DomNode *child = dom->cpd_children;
        while (child) {
            dom_visit(child, f);
            child = child->next;
        }
    }
}

int dom_list_length(struct DomNode* current)
{
    int result = 0;
    while (current) {
    current = current->next;
    ++result;
    }
    return result;
}

static num_nodes;
static void dom_count_callback(struct DomNode* node) { ++num_nodes; }
int dom_tree_count(struct DomNode *dom)
{
    num_nodes = 0;
    dom_visit(dom, dom_count_callback);
    return num_nodes;
}
