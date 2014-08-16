/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef DOM_H
#define DOM_H

#include <stdbool.h>

#include "common.h"
#include "tok.h"

enum DomNodeType { DOM_ATOM, DOM_COMPOUND };
enum DomCpdType { DOM_CPD_CORE, DOM_CPD_ARRAY, DOM_CPD_TUPLE };

struct DomNode {
    /* Data */
    struct Location loc;
    enum DomNodeType type;
    char *atom;
    enum DomCpdType cpd_type;
    struct DomNode *cpd_children;

    /* Intrusive list */
    struct DomNode *next;
};

/* Creation.
 * =========
 */

struct DomNode *dom_make_atom(struct Location loc, char *begin, char *end);
struct DomNode *dom_make_compound(
        struct Location loc,
        enum DomCpdType compound_type,
        struct DomNode *children);

/* Destruction.
 * ============
 */

void dom_free(struct DomNode *dom);

/* Operations.
 * ===========
 */

bool dom_node_is_atom(struct DomNode *node);
bool dom_node_is_spec_atom(struct DomNode *node, char *atom);
bool dom_node_is_compound(struct DomNode *node);
bool dom_node_is_spec_compound(struct DomNode *node, enum DomCpdType type);
bool dom_node_is_cpd_of_size(struct DomNode *node, int size);
bool dom_node_is_cpd_min_size(struct DomNode *node, int size);
char *dom_node_parse_atom(struct DomNode *node);
void dom_visit(struct DomNode *dom, void (*f)(struct DomNode*));

int dom_list_length(struct DomNode *dom);
int dom_tree_count(struct DomNode *dom);

#endif
