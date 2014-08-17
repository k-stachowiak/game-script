/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <string.h>
#include <stdlib.h>

#include "eval.h"
#include "value.h"
#include "bif.h"

/* Function call evaluation.
 * =========================
 */

static struct Value eval_func_call_lookup_value(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    struct Value result;
    struct SymMapKvp *kvp = sym_map_find(sym_map, node->data.func_call.symbol);

    if (!kvp) {
        printf("Symbol \"%s\" not found.\n", node->data.func_call.symbol);
        exit(1);
    }

    result = stack_peek_value(stack, kvp->location);

    if (result.header.type != (uint32_t)VAL_FUNCTION) {
        printf("Symbol \"%s\" not a function.\n", node->data.func_call.symbol);
        exit(1);
    }

    return result;
}

static int eval_func_call_compute_arity(struct AstNode *def_node)
{
    switch (def_node->type) {
    case AST_BIF:
        switch (def_node->data.bif.type) {
        case AST_BIF_ARYTHM_UNARY:
            return 1;
        case AST_BIF_ARYTHM_BINARY:
            return 2;
        default:
            printf("Unhandled bif type.\n");
            exit(1);
        }
        break;

    case AST_FUNC_DEF:
        /* TODO: Deal with the strong separation of the BIF and defs? Have some common data? */
        return def_node->data.func_def.func.arg_count;
        break;

    default:
        printf("Non-function AST node pointed by function value.\n");
        exit(1);
    }
}

static void eval_func_call_curry_on(
        struct AstNode *call_node,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    static uint32_t type = (uint32_t)VAL_FUNCTION;
    static uint32_t zero = 0;
    struct AstNode *arg_node;
    uint32_t applied_count = 0;
    ptrdiff_t size_loc, appl_count_loc, data_begin, data_size;
    uint32_t i;

    /* Header. */
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    size_loc = stack->top;

    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
    data_begin = stack->top;

    /* Pointer to implementation. */
    stack_push(stack, VAL_PTR_BYTES, (char*)&value->function.def);

    /* Captures. */
    stack_push(stack, VAL_SIZE_BYTES, (char*)&value->function.captures.size);
    for (i = 0; i < value->function.captures.size; ++i) {
        struct Capture *cap = value->function.captures.data + i;
        uint32_t len = strlen(cap->symbol);
        struct ValueHeader header = stack_peek_header(stack, cap->location);
        stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
        stack_push(stack, len, cap->symbol);
        stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + cap->location);
    }

    /* Already applied. */
    appl_count_loc = stack->top;
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
    for (i = 0; i < value->function.applied.size; ++i) {
        ptrdiff_t applied_loc = value->function.applied.data[i];
        struct ValueHeader header = stack_peek_header(stack, applied_loc);
        stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + applied_loc);
        ++applied_count;
    }

    /* New applied. */
    arg_node = call_node->data.func_call.actual_args;
    while (arg_node) {
        eval(arg_node, stack, sym_map);
        arg_node = arg_node->next;
        ++applied_count;
    }

    /* Hack value size to correct value. */
    data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_SIZE_BYTES);

    /* Hack applied count to correct value. */
    memcpy(stack->buffer + appl_count_loc, &applied_count, VAL_SIZE_BYTES);
}

static void eval_func_call_final_bif2(
        struct AstNode *call_node, /* TODO: Does this need to take entire AstNode* ? */
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    static uint32_t type_int = (uint32_t)VAL_INT;
    static uint32_t size_int = VAL_INT_BYTES;
    static uint32_t type_real = (uint32_t)VAL_REAL;
    static uint32_t size_real = VAL_REAL_BYTES;
    struct AstBif *impl = &value->function.def->data.bif;
    struct Value args[BIF_MAX_ARITY];
    int arg_count = 0;
    ptrdiff_t temp_begin, temp_end;
    struct AstNode *arg_node;
    uint32_t i;

    /* Peal out already applied args. */
    for (i = 0; i < value->function.applied.size; ++i) {
        ptrdiff_t loc = value->function.applied.data[i];
        args[arg_count++] = stack_peek_value(stack, loc);
    }

    /* Evaluate the missing args. */
    temp_begin = stack->top;
    arg_node = call_node->data.func_call.actual_args;
    while (arg_node) {
        ptrdiff_t loc = stack->top;
        eval(arg_node, stack, sym_map);
        args[arg_count++] = stack_peek_value(stack, loc);
        arg_node = arg_node->next;
    }
    temp_end = stack->top;

    /* Evaluate the function implementation. */
    switch (impl->type) {
    case AST_BIF_ARYTHM_UNARY:
        if ((enum ValueType)args[0].header.type == VAL_INT) {
            long result = impl->un_int_impl(args[0].primitive.integer);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
            stack_push(stack, size_int, (char*)&result);

        } else if ((enum ValueType)args[0].header.type == VAL_REAL) {
            double result = impl->un_real_impl(args[0].primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);

        } else {
            printf("Unhandled BIF argument type.\n");
            exit(1);
        }
        break;

    case AST_BIF_ARYTHM_BINARY:
        if ((enum ValueType)args[0].header.type == VAL_INT &&
            (enum ValueType)args[1].header.type == VAL_INT) {
                long result = impl->bin_int_impl(
                        args[0].primitive.integer,
                        args[1].primitive.integer);
                stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
                stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
                stack_push(stack, size_int, (char*)&result);

        } else if ((enum ValueType)args[0].header.type == VAL_REAL &&
                   (enum ValueType)args[1].header.type == VAL_REAL) {
            double result = impl->bin_real_impl(
                    args[0].primitive.real,
                    args[1].primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);

        } else {
            printf("Unhandled BIF argument type.\n");
            exit(1);
        }
        break;
    }

    /* Collapse the temporaries. */
    stack_collapse(stack, temp_begin, temp_end);
}

void eval_func_call_final_bif(
        struct AstNode *call_node, /* TODO: Does this need to take entire AstNode* ? */
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    static uint32_t type_int = (uint32_t)VAL_INT;
    static uint32_t size_int = VAL_INT_BYTES;
    static uint32_t type_real = (uint32_t)VAL_REAL;
    static uint32_t size_real = VAL_REAL_BYTES;
    ptrdiff_t temp_begin = stack->top, temp_end;
    struct AstBif *impl = &value->function.def->data.bif;
    ptrdiff_t x_loc, y_loc;
    struct Value x, y;

    switch(impl->type) {
    case AST_BIF_ARYTHM_UNARY:

        x_loc = stack->top;
        eval(call_node->data.func_call.actual_args, stack, sym_map);
        x = stack_peek_value(stack, x_loc);

        temp_end = stack->top;
        if ((enum ValueType)x.header.type == VAL_INT) {
            long result = impl->un_int_impl(x.primitive.integer);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
            stack_push(stack, size_int, (char*)&result);
        } else if ((enum ValueType)x.header.type == VAL_REAL) {
            double result = impl->un_real_impl(x.primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);
        } else {
            printf("Unhandled BIF argument type.\n");
            exit(1);
        }
        break;

    case AST_BIF_ARYTHM_BINARY:

        x_loc = stack->top;
        eval(call_node->data.func_call.actual_args, stack, sym_map);
        x = stack_peek_value(stack, x_loc);

        y_loc = stack->top;
        eval(call_node->data.func_call.actual_args->next, stack, sym_map);
        y = stack_peek_value(stack, y_loc);

        temp_end = stack->top;
        if ((enum ValueType)x.header.type == VAL_INT &&
            (enum ValueType)y.header.type == VAL_INT) {
                long result = impl->bin_int_impl(x.primitive.integer, y.primitive.integer);
                stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_int);
                stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_int);
                stack_push(stack, size_int, (char*)&result);
        } else if ((enum ValueType)x.header.type == VAL_REAL &&
                   (enum ValueType)y.header.type == VAL_REAL) {
            double result = impl->bin_real_impl(x.primitive.real, y.primitive.real);
            stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type_real);
            stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size_real);
            stack_push(stack, size_real, (char*)&result);
        } else {
            printf("Unhandled BIF argument type.\n");
            exit(1);
        }
        break;

    default:
        printf("Unhandled BIF type.\n");
        exit(1);
    }

    stack_collapse(stack, temp_begin, temp_end);
}

static void eval_func_call_final_def(
        struct AstNode *call_node,
        struct Stack *stack,
        struct SymMap *sym_map,
        struct Value *value)
{
    uint32_t i;
    ptrdiff_t temp_begin, temp_end;
    struct SymMap local_sym_map;
    struct AstNode *arg_node;
    struct AstNode *exe_node;
    struct AstNode *impl;
    struct AstCommonFunc *func;
    char **formal_arg;

    /* Create the local scope. */
    sym_map_init(&local_sym_map, sym_map, stack);

    /* Insert captures into the scope. */
    for (i = 0; i < value->function.captures.size; ++i) {
        struct Capture *cap = value->function.captures.data + i;
        sym_map_insert(&local_sym_map, cap->symbol, cap->location);
    }

    /* Begin inserting arguments into the scope. */
    impl = value->function.def;
    func = &impl->data.func_def.func;
    formal_arg = func->formal_args;

    /* Insert already applied arguments. */
    for (i = 0; i < value->function.applied.size; ++i) {
        ptrdiff_t loc = value->function.applied.data[i];
        sym_map_insert(&local_sym_map, *formal_arg, loc);
        ++formal_arg;
    }

    /* Insert new arguments. */
    temp_begin = stack->top;
    arg_node = call_node->data.func_call.actual_args;
    while (arg_node) {
        ptrdiff_t loc = stack->top;
        eval(arg_node, stack, sym_map);
        sym_map_insert(&local_sym_map, *formal_arg, loc);
        ++formal_arg;
        arg_node = arg_node->next;
    }
    temp_end = stack->top;

    /* Evaluate the function expression. */
    exe_node = impl->data.func_def.exprs;
    while (exe_node) {
        eval(exe_node, stack, &local_sym_map);
        exe_node = exe_node->next;
    }

    /* Collapse the temporaries. */
    stack_collapse(stack, temp_begin, temp_end);
}

static void eval_func_call(
        struct AstNode *call_node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    struct Value value = eval_func_call_lookup_value(call_node, stack, sym_map);

    int arity = eval_func_call_compute_arity(value.function.def);
    int applied_args = value.function.applied.size;
    int new_args = ast_list_len(call_node->data.func_call.actual_args);

    if (applied_args + new_args < arity) {
        eval_func_call_curry_on(call_node, stack, sym_map, &value);

    } else if (applied_args + new_args == arity) {
        switch (value.function.def->type) {
        case AST_BIF:
            eval_func_call_final_bif2(call_node, stack, sym_map, &value);
            break;

        case AST_FUNC_DEF:
            eval_func_call_final_def(call_node, stack, sym_map, &value);
            break;

        default:
            printf("Non-function AST node pointed by function value.\n");
            exit(1);
        }

    } else {
        printf("Provided too many arguments to a function.\n");
        exit(1);
    }
}

/* Function definition evaluation.
 * ===============================
 */

static bool eval_func_def_copy_non_global(
        struct Stack *stack,
        struct SymMap *sym_map,
        char *symbol)
{
    struct SymMapKvp *kvp = sym_map_find_not_global(sym_map, symbol);
    struct ValueHeader header;
    uint32_t len = strlen(symbol);

    if (!kvp) {
        kvp = sym_map_find(sym_map, symbol);
        if (!kvp) {
            printf("Required symbol not found for capture.\n");
            exit(1);
        } else {
            return false;
        }
    }

    header = stack_peek_header(stack, kvp->location);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&len);
    stack_push(stack, len, symbol);
    stack_push(stack, header.size + VAL_HEAD_BYTES, stack->buffer + kvp->location);

    return true;
}

static bool eval_func_def_is_arg(char *symbol, struct AstCommonFunc *func)
{
    int i;
    for (i = 0; i < func->arg_count; ++i) {
        if (strcmp(symbol, func->formal_args[i]) == 0) {
            return true;
        }
    }
    return false;
}

static void eval_func_def_push_captures(
        struct Stack *stack,
        struct SymMap *sym_map,
        struct AstFuncDef *func_def)
{
    static uint32_t zero = 0;
    ptrdiff_t cap_loc = stack->top;
    uint32_t cap_count = 0;
    struct {
        struct AstNode **data;
        int size, cap;
    } to_visit = { NULL, 0, 0 };

    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);

    ARRAY_APPEND(to_visit, func_def->exprs);
    while (to_visit.size > 0) {

        struct AstNode *n = to_visit.data[0];

        if (n->next) {
            ARRAY_APPEND(to_visit, n->next);
        }

        switch (n->type) {
        case AST_BIF:
        case AST_LITERAL:
            break;

        case AST_BIND:
            ARRAY_APPEND(to_visit, n->data.bind.expr);
            break;

        case AST_COMPOUND:
            ARRAY_APPEND(to_visit, n->data.compound.exprs);
            break;

        case AST_FUNC_DEF:
            ARRAY_APPEND(to_visit, n->data.func_def.exprs);
            break;

        case AST_FUNC_CALL:
            ARRAY_APPEND(to_visit, n->data.func_call.actual_args);
            if (eval_func_def_is_arg(n->data.func_call.symbol, &func_def->func) == false &&
                eval_func_def_copy_non_global(stack, sym_map, n->data.func_call.symbol)) {
                    ++cap_count;
            }
            break;

        case AST_REFERENCE:
            if (eval_func_def_is_arg(n->data.reference.symbol, &func_def->func) == false &&
                eval_func_def_copy_non_global(stack, sym_map, n->data.reference.symbol)) {
                    ++cap_count;
            }
            break;
        }

        ARRAY_REMOVE(to_visit, 0);
    }
    memcpy(stack->buffer + cap_loc, &cap_count, VAL_SIZE_BYTES);
}

static void eval_func_def(
        struct AstNode *node,
        struct Stack *stack,
        struct SymMap *sym_map)
{
    static uint32_t type = (uint32_t)VAL_FUNCTION;
    static uint32_t zero = 0;
    void* impl = (void*)node;
    ptrdiff_t size_loc, data_begin, data_size;

    /* Header. */
    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    size_loc = stack->top;

    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&zero);
    data_begin = stack->top;

    /* Pointer to implementation. */
    stack_push(stack, VAL_PTR_BYTES, (char*)&impl);

    /* Captures. */
    eval_func_def_push_captures(stack, sym_map, &node->data.func_def);

    /* Already applied (empty for definitnion). */
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);

    /* Hack value size to correct value. */
    data_size = stack->top - data_begin;
    memcpy(stack->buffer + size_loc, &data_size, VAL_HEAD_SIZE_BYTES);
}

/* Compound symbol evaluation.
 * ===========================
 */

static ptrdiff_t eval_compound_compute_size(struct AstNode *node)
{
    struct AstNode *child;
    ptrdiff_t cpd_len = 0;

    switch (node->type) {
    case AST_COMPOUND:
        child = node->data.compound.exprs;
        while (child) {
            cpd_len += eval_compound_compute_size(child);
            cpd_len += VAL_HEAD_BYTES;
            child = child->next;
        }
        return cpd_len;

    case AST_LITERAL:
        switch (node->data.literal.type) {
        case AST_LIT_BOOL:
            return VAL_BOOL_BYTES;

        case AST_LIT_CHAR:
            return VAL_CHAR_BYTES;

        case AST_LIT_INT:
            return VAL_INT_BYTES;

        case AST_LIT_REAL:
            return VAL_REAL_BYTES;

        case AST_LIT_STRING:
            return strlen(node->data.literal.data.string);

        default:
            printf("Unhandled literal type.\n");
            exit(1);
        }
        break;

    default:
        printf("Unhandled value type.\n");
        exit(1);
    }
}

static void eval_compound(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    uint32_t type;
    uint32_t size = eval_compound_compute_size(node); /* TODO: Do we have to precompute this ? */
    struct AstNode *current = node->data.compound.exprs;

    switch (node->data.compound.type) {
        case AST_CPD_ARRAY:
            type = VAL_ARRAY;
            break;

        case AST_CPD_TUPLE:
            type = VAL_TUPLE;
            break;
    }

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);

    while (current) {
        eval(current, stack, sym_map);
        current = current->next;
    }
}

/* Simple evaluation functions.
 * ============================
 */

static void eval_literal(struct AstNode *node, struct Stack *stack)
{
    uint32_t type;
    uint32_t size;
    char *value;

    switch (node->data.literal.type) {
    case AST_LIT_BOOL:
        type = (uint32_t)VAL_BOOL;
        size = VAL_BOOL_BYTES;
        value = (char*)&node->data.literal.data.boolean;
        break;

    case AST_LIT_CHAR:
        type = (uint32_t)VAL_CHAR;
        size = VAL_CHAR_BYTES;
        value = (char*)&node->data.literal.data.character;
        break;

    case AST_LIT_INT:
        type = (uint32_t)VAL_INT;
        size = VAL_INT_BYTES;
        value = (char*)&node->data.literal.data.integer;
        break;

    case AST_LIT_REAL:
        type = (uint32_t)VAL_REAL;
        size = VAL_REAL_BYTES;
        value = (char*)&node->data.literal.data.real;
        break;

    case AST_LIT_STRING:
        value = node->data.literal.data.string;
        type = (uint32_t)VAL_STRING;
        size = strlen(value);
        break;
    }

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, size, value);
}

static void eval_bind(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	ptrdiff_t location = eval(node->data.bind.expr, stack, sym_map);
	sym_map_insert(sym_map, node->data.bind.symbol, location);
}

static void eval_reference(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
	struct SymMapKvp *kvp = sym_map_find(sym_map, node->data.reference.symbol);
	struct ValueHeader header;
	uint32_t size;

	if (!kvp) {
		/* TODO: Implement unified runtime error handling (SYMBOL NOT FOUND). */
		printf("Symbol \"%s\" not found.\n", node->data.reference.symbol);
		exit(1);
	}

	header = stack_peek_header(stack, kvp->location);
	size = header.size + VAL_HEAD_BYTES;
	stack_push(stack, size, stack->buffer + kvp->location);
}

static void eval_bif(struct AstNode *node, struct Stack *stack)
{
    static uint32_t type = (uint32_t)VAL_FUNCTION;
    static uint32_t size = VAL_PTR_BYTES + 2 * VAL_SIZE_BYTES;
    static uint32_t zero = 0;
    void* impl = (void*)node;

    stack_push(stack, VAL_HEAD_TYPE_BYTES, (char*)&type);
    stack_push(stack, VAL_HEAD_SIZE_BYTES, (char*)&size);
    stack_push(stack, VAL_PTR_BYTES, (char*)&impl);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
    stack_push(stack, VAL_SIZE_BYTES, (char*)&zero);
}

/* Main evaluation dispatch.
 * =========================
 */

/* TODO: No longer return the ptrdiff here. Just don't return anything. */
/* TODO: What about the function locations? Probably only important for errors. */
ptrdiff_t eval(
		struct AstNode *node,
		struct Stack *stack,
		struct SymMap *sym_map)
{
    ptrdiff_t begin = stack->top;
    switch (node->type) {
    case AST_LITERAL:
        eval_literal(node, stack);
        break;

    case AST_COMPOUND:
        eval_compound(node, stack, sym_map);
        break;

    case AST_BIND:
    	eval_bind(node, stack, sym_map);
    	break;

    case AST_REFERENCE:
    	eval_reference(node, stack, sym_map);
    	break;

    case AST_FUNC_DEF:
        eval_func_def(node, stack, sym_map);
        break;

    case AST_BIF:
        eval_bif(node, stack);
        break;

    case AST_FUNC_CALL:
        eval_func_call(node, stack, sym_map);
        break;

	default:
		printf("Unhandled AST node type.\n");
		exit(1);
    }
    return begin;
}
