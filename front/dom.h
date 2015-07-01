/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef DOM_H
#define DOM_H

#include <stdbool.h>

#include "src_iter.h"

/**
 * Type of the symbolic expression node.
 */
enum DomNodeType {
	DOM_ATOM,
	DOM_COMPOUND
};

/**
 * Type of a compound symbolic expression node.
 * Note that this is an extension to the classical symbolic expressions
 * which only deine one type of a list whereas here like in Clojure
 * additional list types are defined.
 */
enum DomCpdType {
	DOM_CPD_CORE,
	DOM_CPD_ARRAY,
	DOM_CPD_TUPLE
};

/**
 * The non-string list of the reserved words.
 */
enum Reserved {
    DOM_RES_DO,
    DOM_RES_BIND,
    DOM_RES_IF,
    DOM_RES_SWITCH,
    DOM_RES_AND,
    DOM_RES_OR,
    DOM_RES_REF,
    DOM_RES_PEEK,
    DOM_RES_POKE,
    DOM_RES_SUCC,
    DOM_RES_PRED,
    DOM_RES_FUNC,
    DOM_RES_UNIT,
    DOM_RES_TRUE,
    DOM_RES_FALSE,
    DOM_RES_DONTCARE,
};

/**
 * The structure defining the symbolic expression node.
 */
struct DomNode {

    /* Data */
    struct SourceLocation loc;		/** < Location in the input source */
    enum DomNodeType type;			/** < Node type */
    char *atom;						/** < Atom string in case of atomic node */
    enum DomCpdType cpd_type;		/** < Type of compound node if is compound */
    struct DomNode *cpd_children;	/** < List of children if is compound */

    /* Intrusive list */
    struct DomNode *next;			/** < Next pointer facility for lists */
};

/* Creation.
 * =========
 */

/**
 * Creates an atomic symbolic expression node
 *
 * @param loc The location in the source string
 * @param begin The pointer to the begin of the atom string
 * @param end The pointer to the first character beyond the atom string
 */
struct DomNode *dom_make_atom(struct SourceLocation *loc, char *begin, char *end);

/**
 * Creates a compound symbolic expression node
 *
 * @param loc The location in the source string
 * @param compound_type Type of the node to create
 * @param cildren Preallocated list of the children in this node
 */
struct DomNode *dom_make_compound(
        struct SourceLocation *loc,
        enum DomCpdType compound_type,
        struct DomNode *children);

/* Destruction.
 * ============
 */

/**
 * Polymorphic destructor deallocating a symbolic expression node
 * along with any possible successors in an intrusive list.
 */
void dom_free(struct DomNode *dom);

/* Operations.
 * ===========
 */

/** Checks if the node is atomic */
bool dom_node_is_atom(struct DomNode *node);

/** Checks if the node is a given reserved atom */
bool dom_node_is_reserved_atom(struct DomNode *node, enum Reserved res);

/** Checks if the node is of any compound type */
bool dom_node_is_compound(struct DomNode *node);

/** Checks if the node is of a given compound type */
bool dom_node_is_spec_compound(struct DomNode *node, enum DomCpdType type);

/** Checks if the node is of any compound type and given size */
bool dom_node_is_cpd_of_size(struct DomNode *node, int size);

/** Checks if the node is of any compound type and of a given size or greater */
bool dom_node_is_cpd_min_size(struct DomNode *node, int size);

/**
 * Checks if the node is an atom that qualifies as a symbol.
 * This means that it:
 * - is an atom,
 * - isn't delimited
 * - doesn't start with a digit
 * - is not any of the reserved words.
 *
 * @param node Node to be parsed.
 * @returns An internally allocated string containing the symbol
 *          upon test success, NULL otherwise. The client takes
 *          ownership of the string.
 */
char *dom_node_parse_symbol(struct DomNode *node);

#endif

