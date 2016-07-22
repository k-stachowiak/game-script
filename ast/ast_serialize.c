/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "strbuild.h"
#include "log.h"

static char *ast_serialize_symbol(struct AstSymbol *symbol)
{
    /* 0. Overview:
     * The function returns the string representation of the reference node.
     */

    /* 1. The string stored in the AST node is copied. */
    int len = strlen(symbol->symbol);
    char *result = mem_malloc(len + 1);
    memcpy(result, symbol->symbol, len + 1);

    /* 2. The copy of the string is returned along with the ownership. */
    return result;
}

static char *ast_serialize_special_common(char *name, struct AstNode *args)
{
    char *result = NULL;
    char *inner_str = NULL;

    /* 0. Overview:
     * The function returns a string representation of a common special form.
     * Common means that it consists of a name and a list of arguments. The
     * special forms that contain patterns don't fit in this pattern and need
     * custom serialization code.
     */

    /* 1. The symbolic list is opened with an according special form name. */
    str_append(result, "(%s", name);

    /* 2. The arguments are linked and will be serialized together */
    inner_str = ast_serialize(args);
    str_append(result, " %s", inner_str);
    mem_free(inner_str);

    /* 3. The closing parenthesis is appended. */
    str_append(result, ")");

    /* 4. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_special_do(struct AstSpecDo *doo)
{
    char *result = NULL;
    char *expr_string = NULL;

    /* 0. Overview:
     * Returns the string representation of a do expression.
     * The expected form is: (do <expression>)
     */

    /* 1. Serialize the inner expression. It is assumed not to be null
     * which is derived from the do statement definition.
     */
    expr_string = ast_serialize(doo->exprs);
    assert(expr_string);
    str_append(result, "(do %s)", expr_string);
    mem_free(expr_string);

    /* 4. Returns the result passing the ownership along. */
    return result;
}

static char *ast_serialize_special_match(struct AstSpecMatch *match)
{
    char *result = NULL;
    char *expr_string = NULL;
    struct Pattern *key = match->keys;
    struct AstNode *value = match->values;

    /* 0. Overview:
     * Function returning the string representation of a match special form.
     * The expected form is: (match <expression> \(<pattern> <expression>\)+)
     */

    /* 1. The matched expression is appended to the result along with the keyword.
     * The temporary string is released afterwards.
     */
    expr_string = ast_serialize(match->expr);
    assert(expr_string);
    str_append(result, "(match %s", expr_string);
    mem_free(expr_string);

    /* 2. The key-value sequence is appended to the result one by one.
     * It is assumed that there is at least one pattern-expression pair by
     * definition.
     */
    assert(key && value);
    do {
        /* 2.1. The key and the value string representations are generated. */
        char *key_string = pattern_serialize(key);
        char *value_string = ast_serialize(value);

        /* 2.2. An according pair is added to the result string releasing
         * the temporary values afterwards.
         */
        assert(key_string && value_string);
        str_append(result, " %s %s", key_string, value_string);
        mem_free(value_string);
        mem_free(key_string);

        /* 2.3. The linked list iterators for the keys and the values are
         * incremented respectively.
         */
        key = key->next;
        value = value->next;

    } while (key && value);

    /* 3. The closing parenthesis is appended to the result. */
    str_append(result, ")");

    /* 4. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_special_func_def(struct AstSpecFuncDef *func_def)
{
    char *result = NULL;
    char *arg_string = NULL;
    char *expr_string = NULL;

    /* 0. Overview:
     * Function returns a string representation of the given function definition
     * AST node.
     * The expected form is: (func (<pattern>*) <expression>)
     */

    /* 1. The argument list string is generated and appended to the result along
     * with the opening parenthesis and the keyword. The temporary string is
     * released afterwards. The pattern list is optional.
     */
    arg_string = pattern_serialize(func_def->formal_args);
    if (arg_string) {
        str_append(result, "(func (%s)", arg_string);
        mem_free(arg_string);
    }

    /* 2. The function body string is generated and appended to the result.
     * The temporary string is released afterwards. The expression element is
     * obligatory which is subject to assertion.
     */
    expr_string = ast_serialize(func_def->expr);
    assert(expr_string);
    str_append(result, " %s)", expr_string);
    mem_free(expr_string);

    /* 3. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_special_bind(struct AstSpecBind *bind)
{
    char *result = NULL;
    char *pat_string = NULL;
    char *expr_string = NULL;

    /* 0. Overview:
     * Returns the string representation of a bind expression.
     * The expected form is: (bind <pattern> <expression>)
     */

    /* 1. Appends the keyword to the result. */
    str_append(result, "(bind ");

    /* 2. Appends the pattern to the result assumming it is not null by
     * the bind expression definition.
     */
    pat_string = pattern_serialize(bind->pattern);
    assert(pat_string);
    str_append(result, "%s", pat_string);
    mem_free(pat_string);

    /* 3. Appends the expression to the result assumming it is not null by
     * the bind expression definition.
     */
    expr_string = ast_serialize(bind->expr);
    assert(expr_string);
    str_append(result, " %s)", expr_string);
    mem_free(expr_string);

    /* 4. Returns the result passing the ownership along. */
    return result;
}

static char *ast_serialize_special(struct AstSpecial *special)
{
    char *result = NULL;

    /* 0. Overview:
     * The function returns a text representation of a control AST node.
     */

    /* 1. The creation of the particular string is delegated to an according
     * subprocedure.
     */
    switch (special->type) {
    case AST_SPEC_DO:
        return ast_serialize_special_do(&special->data.doo);

    case AST_SPEC_MATCH:
        return ast_serialize_special_match(&special->data.match);

    case AST_SPEC_IF:
        return ast_serialize_special_common("if", special->data.iff.test);

    case AST_SPEC_WHILE:
        return ast_serialize_special_common("while", special->data.whilee.test);

    case AST_SPEC_FUNC_DEF:
        return ast_serialize_special_func_def(&special->data.func_def);

    case AST_SPEC_AND:
        return ast_serialize_special_common("and", special->data.andd.exprs);

    case AST_SPEC_OR:
        return ast_serialize_special_common("or", special->data.orr.exprs);

    case AST_SPEC_BIND:
        return ast_serialize_special_bind(&special->data.bind);

    case AST_SPEC_REF:
        return ast_serialize_special_common("ref", special->data.ref.expr);

    case AST_SPEC_PEEK:
        return ast_serialize_special_common("peek", special->data.peek.expr);

    case AST_SPEC_POKE:
        return ast_serialize_special_common("poke", special->data.poke.reference);

    case AST_SPEC_BEGIN:
        return ast_serialize_special_common("begin", special->data.begin.collection);

    case AST_SPEC_END:
        return ast_serialize_special_common("end", special->data.end.collection);

    case AST_SPEC_INC:
        return ast_serialize_special_common("inc", special->data.inc.reference);

    case AST_SPEC_SUCC:
        return ast_serialize_special_common("succ", special->data.succ.reference);
    }

    /* 2. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_func_call(struct AstFuncCall *func_call)
{
    char *result = NULL;
    char *expr_string = NULL;

    /* 0. Overview:
     * Function returns a string representation of the given function call
     * AST node.
     * The expected form is: (<expression>+)
     * It calls to an explanation as it does not follow an intuition which
     * would be closer to (<function-name> <argument>*). However the language
     * enables using complex expressions as the first element to the function
     * call list. This differentiation is recognized in the runtime and
     * therefore is indistinguishable from the arguments which are optional.
     * The required function expression plus the zero or more argument
     * expressions reduce to one or more expression.
     */

    /* 1. The function expression string is generated and appended to the result.
     * The temporary string is released afterwards. Note that the function
     * expression is linked to the argument expressions therefore in is the only
     * one that we need to serialize here.
     */
    expr_string = ast_serialize(func_call->func);
    str_append(result, "(%s)", expr_string);
    mem_free(expr_string);

    /* 2. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_literal_compound(struct AstLiteralCompound *lit_cpd)
{
    char *result = NULL;
    char *expr_str = NULL;

    /* 0. Overview:
     * The function returns a string representaton of a compound literal node.
     * The compound node is interpreted as a literal expression as it leads
     * dierctly to instantiation of a value, however it is only literal on the top
     * level, as within the defining delimiters there may be arbitrarily complex
     * sub-expressions. */

    /* 1. An opening delimiter is added to the result based on the node type. */
    switch (lit_cpd->type) {
    case AST_LIT_CPD_ARRAY:
        str_append(result, "[");
        break;
    case AST_LIT_CPD_TUPLE:
        str_append(result, "{");
        break;
    }

    /* 2. The list of the sub-expressions is appended to the result if one is
     * present. The temporary string is deleted afterwards.
     */
    if (lit_cpd->exprs) {
        expr_str = ast_serialize(lit_cpd->exprs);
        str_append(result, " %s ", expr_str);
        mem_free(expr_str);
    }

    /* 3. A closing delimiter is added to the result based on the node type. */
    switch (lit_cpd->type) {
    case AST_LIT_CPD_ARRAY:
        str_append(result, "]");
        break;
    case AST_LIT_CPD_TUPLE:
        str_append(result, "}");
        break;
    }

    /* 4. The result is returned passing the ownership. */
    return result;
}

char *ast_serialize_literal_atomic(struct AstLiteralAtomic *literal_atomic)
{
    char *result = NULL;

    /* 0. Overview:
     * The function returns a string representation of a atomic literal node.
     */

    /* 1. The atomic literal nodes are atoms, therefore this function may be implemented
     * in terms of a simple, shallow switch statement.
     */
    switch (literal_atomic->type) {
    case AST_LIT_ATOM_UNIT:
        str_append(result, "unit");
        break;
    case AST_LIT_ATOM_BOOL:
        if (literal_atomic->data.boolean) {
            str_append(result, "true");
        }
        else {
            str_append(result, "false");
        }
        break;
    case AST_LIT_ATOM_STRING:
        str_append(result, "%s", literal_atomic->data.string);
        break;
    case AST_LIT_ATOM_CHAR:
        str_append(result, "%c", literal_atomic->data.character);
        break;
    case AST_LIT_ATOM_INT:
        str_append(result, "%ld", literal_atomic->data.integer);
        break;
    case AST_LIT_ATOM_REAL:
        str_append(result, "%f", literal_atomic->data.real);
        break;
    }

    /* 2. The reseult is returned along with the ownership. */
    return result;
}

char *ast_serialize(struct AstNode *node)
{
    /* 0. Overview:
     * The serialization procedure produces a NUL terminated string containing
     * a text representation of the provided AST.
     */

    char *result = NULL;
    bool first = true;

    /* 1. If NULL was passed, then the empty string must be returned. This
     * accounts for the situation when the function is called for the nested
     * AST element which is something like an empty argument list for a function
     * call.
     */
    if (!node) {
        result = mem_malloc(1);
        *result = '\0';
        return result;
    }

    /* 2. The procedure iterates over a linked list of AST nodes. */
    for (; node; node = node->next) {

        /* 2.1. A temporary string is acquired from the subprocedures depending on
         * the actual type of the current node.
         */
        char *temp = NULL;
        switch (node->type) {
        case AST_SYMBOL:
            temp = ast_serialize_symbol(&node->data.symbol);
            break;

        case AST_SPECIAL:
            temp = ast_serialize_special(&node->data.special);
            break;

        case AST_FUNCTION_CALL:
            temp = ast_serialize_func_call(&node->data.func_call);
            break;

        case AST_LITERAL_COMPOUND:
            temp = ast_serialize_literal_compound(&node->data.literal_compound);
            break;

        case AST_LITERAL_ATOMIC:
            temp = ast_serialize_literal_atomic(&node->data.literal_atomic);
            break;
        }

        /* 2.2. The temporary string is appended to the result with a preceeding
         * space for every element except the first one. This solves the problem
         * of N nodes being separated with N - 1 spaces
         */
        if (first) {
            first = false;
            str_append(result, "%s", temp);
        } else {
            str_append(result, " %s", temp);
        }

        /* 2.3. The teporary string buffer is released. */
        mem_free(temp);
    }

    /* 3. The result is returned along with the ownership. */
    return result;
}

