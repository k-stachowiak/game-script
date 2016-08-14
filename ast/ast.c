/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "memory.h"
#include "ast.h"

struct AstNode *ast_make_symbol(char *symbol)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    int length = strlen(symbol);
    char *symbol_copy = mem_malloc(length + 1);
    memcpy(symbol_copy, symbol, length + 1);

    result->next = NULL;
    result->type = AST_SYMBOL;

    result->data.symbol.symbol = symbol_copy;

    return result;
}

struct AstNode *ast_make_spec_do(struct AstNode* exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_DO;
    result->data.special.data.doo.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_match(
        struct AstNode *expr,
        struct AstNode *keys,
        struct AstNode *values)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_MATCH;
    result->data.special.data.match.expr = expr;
    result->data.special.data.match.keys = keys;
    result->data.special.data.match.values = values;

    /* Link values list to the matched expression to ease traversal. */
    expr->next = values;

    return result;
}

struct AstNode *ast_make_spec_if(
        struct AstNode *test,
        struct AstNode *true_expr,
        struct AstNode *false_expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_IF;
    result->data.special.data.iff.test = test;
    result->data.special.data.iff.true_expr = true_expr;
    result->data.special.data.iff.false_expr = false_expr;

    /* Link to ease releasing */
    test->next = true_expr;
    true_expr->next = false_expr;

    return result;
}

struct AstNode *ast_make_spec_while(
        struct AstNode *test,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_WHILE;
    result->data.special.data.whilee.test = test;
    result->data.special.data.whilee.expr = expr;

    /* Link to ease releasing */
    test->next = expr;

    return result;
}

struct AstNode *ast_make_spec_func_def(
        struct AstNode *formal_args,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_FUNC_DEF;
    result->data.special.data.func_def.formal_args = formal_args;
    result->data.special.data.func_def.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_bool_and(struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_BOOL_AND;
    result->data.special.data.bool_and.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_bool_or(struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_BOOL_OR;
    result->data.special.data.bool_or.exprs = exprs;

    return result;
}

struct AstNode *ast_make_spec_set_of(struct AstNode *types)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_SET_OF;
    result->data.special.data.set_of.types = types;

    return result;
}

struct AstNode *ast_make_spec_range_of(
        struct AstNode *bound_lo,
        struct AstNode *bound_hi)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_RANGE_OF;
    result->data.special.data.range_of.bound_lo = bound_lo;
    result->data.special.data.range_of.bound_hi = bound_hi;

    bound_lo->next = bound_hi;

    return result;
}

struct AstNode *ast_make_spec_array_of(struct AstNode *type)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_ARRAY_OF;
    result->data.special.data.array_of.type = type;

    return result;
}

struct AstNode *ast_make_spec_tuple_of(struct AstNode *types)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_TUPLE_OF;
    result->data.special.data.tuple_of.types = types;

    return result;
}

struct AstNode *ast_make_spec_pointer_to(struct AstNode *type)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_POINTER_TO;
    result->data.special.data.pointer_to.type = type;

    return result;
}

struct AstNode *ast_make_spec_function_type(struct AstNode *types)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_FUNCTION_TYPE;
    result->data.special.data.function_type.types = types;

    return result;
}

struct AstNode *ast_make_spec_type_product(struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_TYPE_PRODUCT;
    result->data.special.data.type_product.args = args;

    return result;
}

struct AstNode *ast_make_spec_type_union(struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_TYPE_UNION;
    result->data.special.data.type_union.args = args;

    return result;
}

struct AstNode *ast_make_spec_bind(
        struct AstNode *pattern,
        struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_BIND;
    result->data.special.data.bind.pattern = pattern;
    result->data.special.data.bind.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_ref(struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_REF;
    result->data.special.data.ref.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_peek(struct AstNode *expr)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_PEEK;
    result->data.special.data.peek.expr = expr;

    return result;
}

struct AstNode *ast_make_spec_poke(
        struct AstNode *reference,
        struct AstNode *value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_POKE;
    result->data.special.data.poke.reference = reference;
    result->data.special.data.poke.value = value;

    /* Link to ease releasing */
    reference->next = value;

    return result;
}

struct AstNode *ast_make_spec_begin(struct AstNode *collection)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_BEGIN;
    result->data.special.data.begin.collection = collection;

    return result;
}

struct AstNode *ast_make_spec_end(struct AstNode *collection)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_END;
    result->data.special.data.end.collection = collection;

    return result;
}

struct AstNode *ast_make_spec_inc(struct AstNode *reference)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_INC;
    result->data.special.data.inc.reference = reference;

    return result;
}

struct AstNode *ast_make_spec_succ(struct AstNode *reference)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_SPECIAL;

    result->data.special.type = AST_SPEC_SUCC;
    result->data.special.data.succ.reference = reference;

    return result;
}

struct AstNode *ast_make_func_call(
    struct AstNode *func,
    struct AstNode *args)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_FUNCTION_CALL;

    result->data.func_call.func = func;
    result->data.func_call.actual_args = args;

    /* Link to ease releasing */
    func->next = args;

    return result;
}

struct AstNode *ast_make_literal_compound(
        enum AstLiteralCompoundType type,
        struct AstNode *exprs)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_COMPOUND;

    result->data.literal_compound.type = type;
    result->data.literal_compound.exprs = exprs;

    return result;
}

struct AstNode *ast_make_literal_atomic_unit(void)
{
    struct AstNode *result = mem_malloc(sizeof(*result));
    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;
    result->data.literal_atomic.type = AST_LIT_ATOM_UNIT;
    return result;
}

struct AstNode *ast_make_literal_atomic_bool(int value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_BOOL;
    result->data.literal_atomic.data.boolean = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_character(char value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_CHAR;
    result->data.literal_atomic.data.character = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_int(long value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_INT;
    result->data.literal_atomic.data.integer = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_real(double value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_REAL;
    result->data.literal_atomic.data.real = value;

    return result;
}

struct AstNode *ast_make_literal_atomic_string(char *value)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    int length = strlen(value);
    char *copy = mem_malloc(length + 1);
    memcpy(copy, value, length + 1);

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_STRING;
    result->data.literal_atomic.data.string = copy;

    return result;
}

struct AstNode *ast_make_literal_atomic_datatype(enum AstLiteralAtomicDataType type)
{
    struct AstNode *result = mem_malloc(sizeof(*result));

    result->next = NULL;
    result->type = AST_LITERAL_ATOMIC;

    result->data.literal_atomic.type = AST_LIT_ATOM_DATATYPE;
    result->data.literal_atomic.data.datatype = type;

    return result;
}

static void ast_symbol_free(struct AstSymbol *symbol)
{
    mem_free(symbol->symbol);
}

static void ast_special_do_free(struct AstSpecDo *doo)
{
    ast_node_free(doo->exprs);
}

static void ast_special_match_free(struct AstSpecMatch *match)
{
    ast_node_free(match->expr);
    ast_node_free(match->keys);
    /* The expression node is linked with the values list. */
}

static void ast_special_if_free(struct AstSpecIf *iff)
{
    ast_node_free(iff->test);
    /* All the nodes in the iff object are linked together. */
}

static void ast_special_while_free(struct AstSpecWhile *whilee)
{
    ast_node_free(whilee->test);
    /* All the nodes in the whilee object are linked together. */
}

static void ast_special_func_def_free(struct AstSpecFuncDef *func_def)
{
    ast_node_free(func_def->formal_args);
    ast_node_free(func_def->expr);
}

static void ast_special_bool_and_free(struct AstSpecBoolAnd *bool_and)
{
    ast_node_free(bool_and->exprs);
}

static void ast_special_bool_or_free(struct AstSpecBoolOr *bool_or)
{
    ast_node_free(bool_or->exprs);
}

static void ast_special_set_of_free(struct AstSpecSetOf *set_of)
{
    ast_node_free(set_of->types);
}

static void ast_special_range_of_free(struct AstSpecRangeOf *range_of)
{
    ast_node_free(range_of->bound_lo);
}

static void ast_special_array_of_free(struct AstSpecArrayOf *array_of)
{
    ast_node_free(array_of->type);
}

static void ast_special_tuple_of_free(struct AstSpecTupleOf *tuple_of)
{
    ast_node_free(tuple_of->types);
}

static void ast_special_pointer_to_free(struct AstSpecPointerTo *pointer_to)
{
    ast_node_free(pointer_to->type);
}

static void ast_special_function_type_free(struct AstSpecFunctionType *function_type)
{
    ast_node_free(function_type->types);
}

static void ast_special_type_product_free(struct AstSpecTypeProduct *type_product)
{
    ast_node_free(type_product->args);
}

static void ast_special_type_union_free(struct AstSpecTypeUnion *type_union)
{
    ast_node_free(type_union->args);
}

static void ast_special_bind_free(struct AstSpecBind *bind)
{
    ast_node_free(bind->pattern);
    ast_node_free_one(bind->expr);
}

static void ast_special_ref_free(struct AstSpecRef *ref)
{
    ast_node_free(ref->expr);
}

static void ast_special_peek_free(struct AstSpecPeek *peek)
{
    ast_node_free(peek->expr);
}

static void ast_special_poke_free(struct AstSpecPoke *poke)
{
    ast_node_free(poke->reference);
    /* All the nodes in the poke object are linked together. */
}

static void ast_special_begin_free(struct AstSpecBegin *begin)
{
    ast_node_free(begin->collection);
}

static void ast_special_end_free(struct AstSpecEnd *end)
{
    ast_node_free(end->collection);
}

static void ast_special_inc_free(struct AstSpecInc *inc)
{
    ast_node_free(inc->reference);
}

static void ast_special_succ_free(struct AstSpecSucc *succ)
{
    ast_node_free(succ->reference);
}

static void ast_special_free(struct AstSpecial *special)
{
    switch (special->type) {
    case AST_SPEC_DO:
        ast_special_do_free(&special->data.doo);
        break;

    case AST_SPEC_MATCH:
        ast_special_match_free(&special->data.match);
        break;

    case AST_SPEC_IF:
        ast_special_if_free(&special->data.iff);
        break;

    case AST_SPEC_WHILE:
        ast_special_while_free(&special->data.whilee);
        break;

    case AST_SPEC_FUNC_DEF:
        ast_special_func_def_free(&special->data.func_def);
        break;

    case AST_SPEC_BOOL_AND:
        ast_special_bool_and_free(&special->data.bool_and);
        break;

    case AST_SPEC_BOOL_OR:
        ast_special_bool_or_free(&special->data.bool_or);
        break;

    case AST_SPEC_SET_OF:
        ast_special_set_of_free(&special->data.set_of);
        break;

    case AST_SPEC_RANGE_OF:
        ast_special_range_of_free(&special->data.range_of);
        break;

    case AST_SPEC_ARRAY_OF:
        ast_special_array_of_free(&special->data.array_of);
        break;

    case AST_SPEC_TUPLE_OF:
        ast_special_tuple_of_free(&special->data.tuple_of);
        break;

    case AST_SPEC_POINTER_TO:
        ast_special_pointer_to_free(&special->data.pointer_to);
        break;

    case AST_SPEC_FUNCTION_TYPE:
        ast_special_function_type_free(&special->data.function_type);
        break;

    case AST_SPEC_TYPE_PRODUCT:
        ast_special_type_product_free(&special->data.type_product);
        break;

    case AST_SPEC_TYPE_UNION:
        ast_special_type_union_free(&special->data.type_union);
        break;

    case AST_SPEC_BIND:
        ast_special_bind_free(&special->data.bind);
        break;

    case AST_SPEC_REF:
        ast_special_ref_free(&special->data.ref);
        break;

    case AST_SPEC_PEEK:
        ast_special_peek_free(&special->data.peek);
        break;

    case AST_SPEC_POKE:
        ast_special_poke_free(&special->data.poke);
        break;

    case AST_SPEC_BEGIN:
        ast_special_begin_free(&special->data.begin);
        break;

    case AST_SPEC_END:
        ast_special_end_free(&special->data.end);
        break;

    case AST_SPEC_INC:
        ast_special_inc_free(&special->data.inc);
        break;

    case AST_SPEC_SUCC:
        ast_special_succ_free(&special->data.succ);
        break;
    }
}

static void ast_func_call_free(struct AstFuncCall *fcall)
{
    ast_node_free(fcall->func);
    /* All the nodes in the function call object are linked together. */
}

static void ast_literal_compound_free(struct AstLiteralCompound *lit_cpd)
{
    ast_node_free(lit_cpd->exprs);
}

static void ast_literal_atomic_free(struct AstLiteralAtomic *lit_atom)
{
    if (lit_atom->type == AST_LIT_ATOM_STRING) {
        mem_free(lit_atom->data.string);
    }
}

void ast_node_free_one(struct AstNode *node)
{
    switch (node->type) {
    case AST_SYMBOL:
        ast_symbol_free(&node->data.symbol);
        break;

    case AST_SPECIAL:
        ast_special_free(&node->data.special);
        break;

    case AST_FUNCTION_CALL:
        ast_func_call_free(&node->data.func_call);
        break;

    case AST_LITERAL_COMPOUND:
        ast_literal_compound_free(&node->data.literal_compound);
        break;

    case AST_LITERAL_ATOMIC:
        ast_literal_atomic_free(&node->data.literal_atomic);
        break;
    }

    mem_free(node);
}

void ast_node_free(struct AstNode *current)
{
    while (current) {
        struct AstNode *next = current->next;
        ast_node_free_one(current);
        current = next;
    }
}

int ast_list_len(struct AstNode *head)
{
    int result = 0;
    while (head) {
        head = head->next;
        ++result;
    }
    return result;
}

bool ast_list_contains_symbol(struct AstNode *list, char *symbol)
{
    while (list) {
        switch (list->type) {
        case AST_SYMBOL:
            return strcmp(list->data.symbol.symbol, symbol) == 0;

        case AST_SPECIAL:
            switch (list->data.special.type) {
            case AST_SPEC_DO:
                return ast_list_contains_symbol(list->data.special.data.doo.exprs, symbol);
            case AST_SPEC_MATCH:
                return
                    ast_list_contains_symbol(list->data.special.data.match.expr, symbol) |
                    ast_list_contains_symbol(list->data.special.data.match.keys, symbol) |
                    ast_list_contains_symbol(list->data.special.data.match.values, symbol);
            case AST_SPEC_IF:
                return
                    ast_list_contains_symbol(list->data.special.data.iff.test, symbol) |
                    ast_list_contains_symbol(list->data.special.data.iff.true_expr, symbol) |
                    ast_list_contains_symbol(list->data.special.data.iff.false_expr, symbol);
            case AST_SPEC_WHILE:
                return
                    ast_list_contains_symbol(list->data.special.data.whilee.test, symbol) |
                    ast_list_contains_symbol(list->data.special.data.whilee.expr, symbol);
            case AST_SPEC_FUNC_DEF:
                return
                    ast_list_contains_symbol(list->data.special.data.func_def.formal_args, symbol) |
                    ast_list_contains_symbol(list->data.special.data.func_def.expr, symbol);
            case AST_SPEC_BOOL_AND:
                return ast_list_contains_symbol(list->data.special.data.bool_and.exprs, symbol);
            case AST_SPEC_BOOL_OR:
                return ast_list_contains_symbol(list->data.special.data.bool_or.exprs, symbol);
            case AST_SPEC_SET_OF:
                return ast_list_contains_symbol(list->data.special.data.set_of.types, symbol);
            case AST_SPEC_RANGE_OF:
                return ast_list_contains_symbol(list->data.special.data.range_of.bound_lo, symbol);
            case AST_SPEC_ARRAY_OF:
                return ast_list_contains_symbol(list->data.special.data.array_of.type, symbol);
            case AST_SPEC_TUPLE_OF:
                return ast_list_contains_symbol(list->data.special.data.tuple_of.types, symbol);
            case AST_SPEC_POINTER_TO:
                return ast_list_contains_symbol(list->data.special.data.pointer_to.type, symbol);
            case AST_SPEC_FUNCTION_TYPE:
                return ast_list_contains_symbol(list->data.special.data.function_type.types, symbol);
            case AST_SPEC_TYPE_PRODUCT:
                return ast_list_contains_symbol(list->data.special.data.type_product.args, symbol);
            case AST_SPEC_TYPE_UNION:
                return ast_list_contains_symbol(list->data.special.data.type_union.args, symbol);
            case AST_SPEC_BIND:
                return
                    ast_list_contains_symbol(list->data.special.data.bind.pattern, symbol) |
                    ast_list_contains_symbol(list->data.special.data.bind.expr, symbol);
            case AST_SPEC_REF:
                return ast_list_contains_symbol(list->data.special.data.ref.expr, symbol);
            case AST_SPEC_PEEK:
                return ast_list_contains_symbol(list->data.special.data.peek.expr, symbol);
            case AST_SPEC_POKE:
                return
                    ast_list_contains_symbol(list->data.special.data.poke.reference, symbol) |
                    ast_list_contains_symbol(list->data.special.data.poke.value, symbol);
            case AST_SPEC_BEGIN:
                return ast_list_contains_symbol(list->data.special.data.begin.collection, symbol);
            case AST_SPEC_END:
                return ast_list_contains_symbol(list->data.special.data.end.collection, symbol);
            case AST_SPEC_INC:
                return ast_list_contains_symbol(list->data.special.data.inc.reference, symbol);
            case AST_SPEC_SUCC:
                return ast_list_contains_symbol(list->data.special.data.succ.reference, symbol);
            }
        case AST_FUNCTION_CALL:
            return
                ast_list_contains_symbol(list->data.func_call.func, symbol) |
                ast_list_contains_symbol(list->data.func_call.actual_args, symbol);
        case AST_LITERAL_COMPOUND:
            return ast_list_contains_symbol(list->data.literal_compound.exprs, symbol);
        case AST_LITERAL_ATOMIC:
        default:
            return false;
        }
        list = list->next;
    }
    return false;
}

