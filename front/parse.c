/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "parse.h"
#include "tok.h"
#include "dom.h"

static char *find(char * current, char *last, char value)
{
    while (current != last) {
        if (*current == value) {
            return current;
        }
        ++current;
    }

    return current;
}

static bool all_of(char *current, char *last, int (*f)(int))
{
    while (current != last) {
        if (!f(*current)) {
            return false;
        }
        ++current;
    }
    return true;
}

static struct AstNode *parse_do_block(struct DomNode *dom)
{
	struct AstNode *exprs = NULL;
	struct AstNode *exprs_end = NULL;
	struct DomNode *child = NULL;

	LOG_TRACE_FUNC

	/* 1. Is compound CORE. */
	if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
		return NULL;
	}

	/* 2. Has 2 or more children. */
	if (!dom_node_is_cpd_min_size(dom, 2)) {
		return NULL;
	}

	child = dom->cpd_children;

	/* 3.1. 1st child is "do" keyword. */
	if (!dom_node_is_reserved_atom(child, DOM_RES_DO)) {
		return NULL;
	}
	child = child->next;

	/* 3.2. Has 1 or more further expressions. */
	while (child) {
		struct AstNode *expr = parse(child);
		if (!expr) {
			goto fail;
		}
		else {
			LIST_APPEND(expr, &exprs, &exprs_end);
		}
		child = child->next;
	}

	return ast_make_do_block(dom->loc, exprs);

fail:

	if (exprs) {
		ast_node_free_list(exprs);
	}

	return NULL;
}

static struct AstNode *parse_bind(struct DomNode *dom)
{
    struct DomNode *child = NULL;
    char *symbol = NULL;
    struct AstNode *expr = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound CORE */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is bind keyword. */
    if (!dom_node_is_reserved_atom(child, DOM_RES_BIND)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. 2nd child is symbol. */
	if (!(symbol = dom_node_parse_symbol(child))) {
        return NULL;
    }
    child = child->next;

    /* 3.3 3rd child is any expression. */
    if (!(expr = parse(child))) {
        free(symbol);
        return NULL;
    }

    return ast_make_bind(dom->loc, symbol, expr);
}

static struct AstNode *parse_iff(struct DomNode *dom)
{
	struct DomNode *child = NULL;
	struct AstNode *test = NULL;
	struct AstNode *true_expr = NULL;
	struct AstNode *false_expr = NULL;

	/* 1. Is compound CORE */
	if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
		return NULL;
	}

	/* 2. Has 4 children */
	if (!dom_node_is_cpd_of_size(dom, 4)) {
		return NULL;
	}

	child = dom->cpd_children;

	/* 3.1. 1st child is if keyword. */
	if (!dom_node_is_reserved_atom(child, DOM_RES_IF)) {
		return NULL;
	}
	child = child->next;

	/* 3.2. 2nd child is the test expression. */
	if (!(test = parse(child))) {
		goto fail;
	}
	child = child->next;

	/* 3.3. 3rd child is the true expression. */
	if (!(true_expr = parse(child))) {
		goto fail;
	}
	child = child->next;

	/* 3.4. 4th child is the false expression. */
	if (!(false_expr = parse(child))) {
		goto fail;
	}

	/* NOTE: The expressions are linked here which is useful in a remote place
	 * in the system. Sorry.
	 */
	test->next = true_expr;
	true_expr->next = false_expr;

	return ast_make_iff(dom->loc, test, true_expr, false_expr);

fail:
	if (test) {
		ast_node_free_one(test);
	}

	if (true_expr) {
		ast_node_free_one(true_expr);
	}

	if (false_expr) {
		ast_node_free_one(false_expr);
	}

	return NULL;
}

static struct AstNode *parse_compound(struct DomNode *dom)
{
    enum AstCompoundType type;
    struct AstNode *exprs = NULL;
    struct AstNode *exprs_end = NULL;
    struct DomNode *child = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound. */
    if (dom_node_is_atom(dom)) {
        return NULL;
    }

    /* 2. Is of ARRAY or TUPLE type. */
    switch (dom->cpd_type) {
    case DOM_CPD_ARRAY:
        type = AST_CPD_ARRAY;
        break;

    case DOM_CPD_TUPLE:
        type = AST_CPD_TUPLE;
        break;

    case DOM_CPD_CORE:
	default:
        return NULL;
    }

    /* 3. Has 0 or more expressions. */
    child = dom->cpd_children;
    while (child) {
        struct AstNode *node = parse(child);
        if (!node) {
            goto fail;
        } else {
            LIST_APPEND(node, &exprs, &exprs_end);
        }
        child = child->next;
    }

    return ast_make_compound(dom->loc, type, exprs);

fail:
	if (exprs) {
		ast_node_free_list(exprs);
	}

	return NULL;
}

static struct AstNode *parse_func_call(struct DomNode *dom)
{
    char *symbol = NULL;
    struct AstNode *args = NULL;
    struct AstNode *args_end = NULL;
    struct DomNode *child = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 1 or more children. */
    if (!dom_node_is_cpd_min_size(dom, 1)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is symbol. */
	if (!(symbol = dom_node_parse_symbol(child))) {
        return NULL;
    }
    child = child->next;

    /* 3.2. Has 0 or more further children being any expression. */
    while (child) {
        struct AstNode *node = parse(child);
        if (!node) {
            goto fail;
        } else {
            LIST_APPEND(node, &args, &args_end);
        }
        child = child->next;
    }

    return ast_make_func_call(dom->loc, symbol, args);

fail:
	if (args) {
		ast_node_free_list(args);
	}

	return NULL;
}

static struct AstNode *parse_func_def(struct DomNode *dom)
{
    char **formal_args = NULL;
    int arg_count = 0;
    struct AstNode *expr = NULL;

    struct DomNode *child = NULL;
    struct DomNode *arg_child = NULL;

    LOG_TRACE_FUNC

    /* 1. Is compound CORE. */
    if (!dom_node_is_spec_compound(dom, DOM_CPD_CORE)) {
        return NULL;
    }

    /* 2. Has 3 children. */
    if (!dom_node_is_cpd_of_size(dom, 3)) {
        return NULL;
    }

    child = dom->cpd_children;

    /* 3.1. 1st child is "func" keyword. */
    if (!dom_node_is_reserved_atom(child, DOM_RES_FUNC)) {
        return NULL;
    }
    child = child->next;

    /* 3.2. 2nd keyword is a core compound of symbols. */
    if (!dom_node_is_spec_compound(child, DOM_CPD_CORE)) {
        return NULL;
    }

    arg_child = child->cpd_children;

    /* Argument list may be empty. */
    if (arg_child) {
		formal_args = malloc_or_die(2048 * sizeof(*formal_args));
        while (arg_child) {
            if (!dom_node_is_atom(arg_child)) {
                goto fail;
            } else {
                int len = strlen(arg_child->atom);
				formal_args[arg_count] = malloc_or_die(len + 1);
                memcpy(formal_args[arg_count], arg_child->atom, len + 1);
                ++arg_count;
            }
            arg_child = arg_child->next;
        }
		formal_args = realloc_or_die(formal_args, arg_count * sizeof(*formal_args));
    }

    child = child->next;

    /* 3.3. Has 1 more further expression. */
	if (!(expr = parse(child))) {
		goto fail;
	}

    return ast_make_func_def(dom->loc, formal_args, arg_count, expr);

fail:
    if (formal_args) {
        int i;
        for (i = 0; i < arg_count; ++i) {
            free(formal_args + i);
        }
        free(formal_args);
    }

    if (expr) {
        ast_node_free_one(expr);
    }

    return NULL;
}

static struct AstNode *parse_literal_bool(struct DomNode *dom)
{
    LOG_TRACE_FUNC

    if (dom_node_is_reserved_atom(dom, DOM_RES_TRUE)) {
        return ast_make_literal_bool(dom->loc, 1);
    } else if (dom_node_is_reserved_atom(dom, DOM_RES_FALSE)) {
        return ast_make_literal_bool(dom->loc, 0);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_string(struct DomNode *dom)
{
    char *atom, *atom_cpy;
    int len;
    char delim = TOK_DELIM_STR;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (atom[0] == delim && atom[len - 1] == delim && len >= 2) {
		atom_cpy = malloc_or_die(len + 1);
        memcpy(atom_cpy, atom, len + 1);
        atom_cpy[len] = '\0';
        return ast_make_literal_string(dom->loc, atom_cpy);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_char(struct DomNode *dom)
{
    char *atom;
    int len;
    char delim = TOK_DELIM_CHAR;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (atom[0] != delim || atom[len - 1] != delim) {
        return NULL;
    }

    if (len == 3) {
        return ast_make_literal_character(dom->loc, atom[1]);

    } else if (len == 4 && atom[1] == '\\') {
        switch(atom[2]) {
         case 'b':
            return ast_make_literal_character(dom->loc, '\b');
         case 't':
            return ast_make_literal_character(dom->loc, '\t');
         case 'n':
            return ast_make_literal_character(dom->loc, '\n');
         case 'f':
            return ast_make_literal_character(dom->loc, '\f');
         case 'r':
            return ast_make_literal_character(dom->loc, '\r');
         case '\\':
            return ast_make_literal_character(dom->loc, '\\');
         case '"':
            return ast_make_literal_character(dom->loc, '\"');
         case '\'':
            return ast_make_literal_character(dom->loc, '\'');
         default:
            return NULL;
        }

    } else {
        err_set(ERR_PARSE, "Char literal of incorrect length.");
        return NULL;
    }
}

static struct AstNode *parse_literal_int(struct DomNode *dom)
{
    char *atom;
    int len;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    if (len > 0 && (isdigit(atom[0]) || atom[0] == '-' || atom[0] == '+') &&
        all_of(atom, atom + len, isdigit)) {
        long value = atol(atom);
        return ast_make_literal_int(dom->loc, value);
    } else {
        return NULL;
    }
}

static struct AstNode *parse_literal_real(struct DomNode *dom)
{
    char *atom, *first, *period, *last;
    double value;
    int len;

    LOG_TRACE_FUNC

    if (!dom_node_is_atom(dom)) {
        return NULL;
    }

    atom = dom->atom;
    len = strlen(atom);

    first = atom;
    last = first + len;
    period = find(first, last, '.');

    if (period == last ||
        !all_of(first, period, isdigit) ||
        !all_of(period + 1, last, isdigit)) {
        return NULL;
    }

    value = atof(atom);
    return ast_make_literal_real(dom->loc, value);
}

static struct AstNode *parse_literal(struct DomNode *dom)
{
    struct AstNode *result;

    LOG_TRACE_FUNC

    if ((!err_state() && (result = parse_literal_bool(dom))) ||
        (!err_state() && (result = parse_literal_char(dom))) ||
        (!err_state() && (result = parse_literal_real(dom))) ||
        (!err_state() && (result = parse_literal_int(dom))) ||
        (!err_state() && (result = parse_literal_string(dom)))) {
        return result;

    } else {
        return NULL;
    }
}

static struct AstNode *parse_reference(struct DomNode *dom)
{
    char *symbol;

    LOG_TRACE_FUNC

    /* 1. Is a symbol. */
	if (!(symbol = dom_node_parse_symbol(dom))) {
        return NULL;
    }

    return ast_make_reference(dom->loc, symbol);
}

struct AstNode *parse(struct DomNode *dom)
{
    struct DomNode *current = dom;
    struct AstNode *result = NULL;
    struct AstNode *result_end = NULL;

    LOG_TRACE_FUNC

    err_reset();
    while (current) {
        struct AstNode *node;
        if (
            (!err_state() && (node = parse_literal(dom))) ||
            (!err_state() && (node = parse_reference(dom))) ||
			(!err_state() && (node = parse_do_block(dom))) ||
            (!err_state() && (node = parse_bind(dom))) ||
            (!err_state() && (node = parse_iff(dom))) ||
            (!err_state() && (node = parse_func_def(dom))) ||
            (!err_state() && (node = parse_func_call(dom))) ||
            (!err_state() && (node = parse_compound(dom)))) {

            LIST_APPEND(node, &result, &result_end);

        } else {
            if (!err_state()) {
                err_set(ERR_PARSE, "Failed parsing any AST node.");
            }
            ast_node_free_list(result);
            return NULL;
        }
        current = current->next;
    }

    return result;
}

