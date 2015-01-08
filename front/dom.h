/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef DOM_H
#define DOM_H

#include <stdbool.h>

#include "src_iter.h"

enum DomNodeType { DOM_ATOM, DOM_COMPOUND };
enum DomCpdType { DOM_CPD_CORE, DOM_CPD_ARRAY, DOM_CPD_TUPLE };

enum Reserved {
	DOM_RES_DO,
	DOM_RES_BIND,
	DOM_RES_IF,
	DOM_RES_FUNC,
	DOM_RES_TRUE,
	DOM_RES_FALSE
};

struct DomNode {
    /* Data */
    struct SourceLocation loc;
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

struct DomNode *dom_make_atom(struct SourceLocation *loc, char *begin, char *end);
struct DomNode *dom_make_compound(
        struct SourceLocation *loc,
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
bool dom_node_is_reserved_atom(struct DomNode *node, enum Reserved res);
bool dom_node_is_compound(struct DomNode *node);
bool dom_node_is_spec_compound(struct DomNode *node, enum DomCpdType type);
bool dom_node_is_cpd_of_size(struct DomNode *node, int size);
bool dom_node_is_cpd_min_size(struct DomNode *node, int size);
char *dom_node_parse_symbol(struct DomNode *node);

#endif
