/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include "eval_detail.h"
#include "eval.h"
#include "error.h"
#include "rt_val.h"

void eval_special_set_of(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size;
    struct AstSpecSetOf *set_of = &node->data.special.data.set_of;
    struct AstNode *type = set_of->types;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_SET_OF, &size_loc);

    data_begin = rt->stack.top;
    while (type) {
        eval_dispatch(type, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Failed evaluating type in set_of constructor");
            return;
        } else {
            type = type->next;
        }
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_range_of(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size, from_loc, to_loc;
    struct AstSpecRangeOf *range_of = &node->data.special.data.range_of;
    struct AstNode *bound_lo = range_of->bound_lo;
    struct AstNode *bound_hi = range_of->bound_hi;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_RANGE_OF, &size_loc);

    data_begin = rt->stack.top;

    from_loc = eval_dispatch(bound_lo, rt, sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, bound_lo),
            "Failed evaluating from bound in range_of constructor");
        return;
    }

    if (rt_val_peek_type(&rt->stack, from_loc) != VAL_INT) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, bound_lo),
            "Non-integral from bound in range_of constructor");
        return;
    }

    to_loc = eval_dispatch(bound_hi, rt, sym_map, alm);
    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, bound_hi),
            "Failed evaluating to bound in range_of constructor");
        return;
    }

    if (rt_val_peek_type(&rt->stack, to_loc) != VAL_INT) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, bound_hi),
            "Non-integral to bound in range_of constructor");
        return;
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_array_of(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size, elem_loc;
    struct AstSpecArrayOf *array_of = &node->data.special.data.array_of;
    struct AstNode *type = array_of->type;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_ARRAY_OF, &size_loc);

    data_begin = rt->stack.top;
    elem_loc = eval_dispatch(type, rt, sym_map, alm);

    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, type),
            "Failed evaluating type in array_of constructor");
        return;
    }

    if (rt_val_peek_type(&rt->stack, elem_loc) != VAL_DATATYPE) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, type),
            "Non-type item in array_of constructor");
        return;
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_tuple_of(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size, elem_loc;
    struct AstSpecTupleOf *tuple_of = &node->data.special.data.tuple_of;
    struct AstNode *type = tuple_of->types;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_TUPLE_OF, &size_loc);

    data_begin = rt->stack.top;
    while (type) {
        elem_loc = eval_dispatch(type, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Failed evaluating type in tuple_of constructor");
            return;
        } else if (rt_val_peek_type(&rt->stack, elem_loc) != VAL_DATATYPE) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Non-type item in tuple_of constructor");
            return;
        } else {
            type = type->next;
        }
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_pointer_to(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size, elem_loc;
    struct AstSpecPointerTo *pointer_to = &node->data.special.data.pointer_to;
    struct AstNode *type = pointer_to->type;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_PTR_TO, &size_loc);

    data_begin = rt->stack.top;
    elem_loc = eval_dispatch(type, rt, sym_map, alm);

    if (err_state()) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, type),
            "Failed evaluating type in pointer_to constructor");
        return;
    }

    if (rt_val_peek_type(&rt->stack, elem_loc) != VAL_DATATYPE) {
        err_push_src(
            "EVAL",
            alm_try_get(alm, type),
            "Non-type item in pointer_to constructor");
        return;
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_function_type(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm)
{
    VAL_LOC_T size_loc, data_begin, data_size, elem_loc;
    struct AstSpecFunctionType *function_type = &node->data.special.data.function_type;
    struct AstNode *type = function_type->types;

    rt_val_push_datatype_init(&rt->stack, VAL_EMB_FUNC, &size_loc);

    data_begin = rt->stack.top;
    while (type) {
        elem_loc = eval_dispatch(type, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Failed evaluating type in tuple_of constructor");
            return;
        } else if (rt_val_peek_type(&rt->stack, elem_loc) != VAL_DATATYPE) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Non-type item in tuple_of constructor");
            return;
        } else {
            type = type->next;
        }
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}

void eval_special_type_op(
        struct AstNode *node,
        struct Runtime *rt,
        struct SymMap *sym_map,
        struct AstLocMap *alm,
        bool is_product)
{
    VAL_LOC_T size_loc, data_begin, data_size, elem_loc;
    struct AstSpecFunctionType *function_type = &node->data.special.data.function_type;
    struct AstNode *type = function_type->types;

    type = is_product
        ? node->data.special.data.type_product.args
        : node->data.special.data.type_union.args;

    if (is_product) {
        rt_val_push_datatype_init(&rt->stack, VAL_EMB_PROD, &size_loc);
    } else {
        rt_val_push_datatype_init(&rt->stack, VAL_EMB_UNION, &size_loc);
    }

    data_begin = rt->stack.top;
    while (type) {
        elem_loc = eval_dispatch(type, rt, sym_map, alm);
        if (err_state()) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Failed evaluating type in type operator args");
            return;
        } else if (rt_val_peek_type(&rt->stack, elem_loc) != VAL_DATATYPE) {
            err_push_src(
                "EVAL",
                alm_try_get(alm, type),
                "Non-type item in type operator args");
            return;
        } else {
            type = type->next;
        }
    }

    data_size = rt->stack.top - data_begin;
    rt_val_push_datatype_final(&rt->stack, size_loc, data_size);
}
