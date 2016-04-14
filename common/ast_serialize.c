#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "strbuild.h"
#include "log.h"

static char *ast_serialize_control_do(struct AstCtlDo *doo)
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

static char *ast_serialize_control_bind(struct AstCtlBind *bind)
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

static char *ast_serialize_control_match(struct AstCtlMatch *match)
{
    char *result = NULL;
    char *expr_string = NULL;
    struct Pattern *key = match->keys;
    struct AstNode *value = match->values;

    /* 0. Overview:
     * Function returning the string representation of a match parafunction.
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

static char *ast_serialize_control_fdef(struct AstCtlFuncDef *fdef)
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
    arg_string = pattern_serialize(fdef->formal_args);
    if (arg_string) {
        str_append(result, "(func (%s)", arg_string);
        mem_free(arg_string);
    }

    /* 2. The function body string is generated and appended to the result.
     * The temporary string is released afterwards. The expression element is
         * obligatory which is subject to assertion.
     */
    expr_string = ast_serialize(fdef->expr);
    assert(expr_string);
    str_append(result, " %s)", expr_string);
    mem_free(expr_string);

    /* 3. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_control_fcall(struct AstCtlFuncCall *fcall)
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
    expr_string = ast_serialize(fcall->func);
    str_append(result, "(%s)", expr_string);
    mem_free(expr_string);

    /* 2. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_control_reference(struct AstCtlReference *reference)
{
    /* 0. Overview:
     * The function returns the string representation of the reference node.
     */

    /* 1. The string stored in the AST node is copied. */
    int len = strlen(reference->symbol);
    char *result = mem_malloc(len + 1);
    memcpy(result, reference->symbol, len + 1);

    /* 2. The copy of the string is returned along with the ownership. */
    return result;
}

static char *ast_serialize_control(struct AstControl *control)
{
    char *result = NULL;

    /* 0. Overview:
     * The function returns a text representation of a control AST node.
     */

    /* 1. The creation of the particular string is delegated to an according
     * subprocedure.
     */
    switch (control->type) {
    case AST_CTL_DO:
        result = ast_serialize_control_do(&control->data.doo);
        break;

    case AST_CTL_BIND:
        result = ast_serialize_control_bind(&control->data.bind);
        break;

    case AST_CTL_MATCH:
        result = ast_serialize_control_match(&control->data.match);
        break;

    case AST_CTL_FUNC_DEF:
        result = ast_serialize_control_fdef(&control->data.fdef);
        break;

    case AST_CTL_FUNC_CALL:
        result = ast_serialize_control_fcall(&control->data.fcall);
        break;

    case AST_CTL_REFERENCE:
        result = ast_serialize_control_reference(&control->data.reference);
        break;
    }

    /* 2. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_parafunc(struct AstParafunc *parafunc)
{
    char *result = NULL;
    char *child_str = NULL;

    /* 0. Overview:
     * The function returns a string representation of the given parafunction.
     */

    /* 1. The symbolic list is opened with an according parafunction name. */
    switch (parafunc->type) {
    case AST_PARAFUNC_IF:
        str_append(result, "(if");
        break;
    case AST_PARAFUNC_WHILE:
        str_append(result, "(while");
        break;
    case AST_PARAFUNC_AND:
        str_append(result, "(and");
        break;
    case AST_PARAFUNC_OR:
        str_append(result, "(or");
        break;
    case AST_PARAFUNC_REF:
        str_append(result, "(ref");
        break;
    case AST_PARAFUNC_PEEK:
        str_append(result, "(peek");
        break;
    case AST_PARAFUNC_POKE:
        str_append(result, "(poke");
        break;
    case AST_PARAFUNC_BEGIN:
        str_append(result, "(begin");
        break;
    case AST_PARAFUNC_END:
        str_append(result, "(end");
        break;
    case AST_PARAFUNC_INC:
        str_append(result, "(inc");
        break;
    case AST_PARAFUNC_SUCC:
        str_append(result, "(succ");
        break;
    }

    /* 2. The arguments to the parafunction are appended to the result if such
     * are present. Afterwards the temporary string is released.
     */
    if (parafunc->args) {
        child_str = ast_serialize(parafunc->args);
        str_append(result, " %s", child_str);
        mem_free(child_str);
    }

    /* 3. The closing parenthesis is appended. */
    str_append(result, ")");

    /* 4. The result is returned passing the ownership. */
    return result;
}

static char *ast_serialize_compound(struct AstCompound *compound)
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
    switch (compound->type) {
    case AST_CPD_ARRAY:
        str_append(result, "[");
        break;
    case AST_CPD_TUPLE:
        str_append(result, "{");
        break;
    }

    /* 2. The list of the sub-expressions is appended to the result if one is
     * present. The temporary string is deleted afterwards.
     */
    if (compound->exprs) {
        expr_str = ast_serialize(compound->exprs);
        str_append(result, " %s ", expr_str);
        mem_free(expr_str);
    }

    /* 3. A closing delimiter is added to the result based on the node type. */
    switch (compound->type) {
    case AST_CPD_ARRAY:
        str_append(result, "]");
        break;
    case AST_CPD_TUPLE:
        str_append(result, "}");
        break;
    }

    /* 4. The result is returned passing the ownership. */
    return result;
}

char *ast_serialize_literal(struct AstLiteral *literal)
{
    char *result = NULL;

    /* 0. Overview:
     * The function returns a string representation of a literal node.
     */

    /* 1. The literal nodes are atoms, therefore this function may be implemented
     * in terms of a simple, shallow switch statement.
     */
    switch (literal->type) {
    case AST_LIT_UNIT:
        str_append(result, "unit");
        break;
    case AST_LIT_BOOL:
        if (literal->data.boolean) {
            str_append(result, "true");
        }
        else {
            str_append(result, "false");
        }
        break;
    case AST_LIT_STRING:
        str_append(result, "%s", literal->data.string);
        break;
    case AST_LIT_CHAR:
        str_append(result, "%c", literal->data.character);
        break;
    case AST_LIT_INT:
        str_append(result, "%ld", literal->data.integer);
        break;
    case AST_LIT_REAL:
        str_append(result, "%f", literal->data.real);
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
    for (; node; node = node->next)    {

        /* 2.1. A temporary string is acquired from the subprocedures depending on
         * the actual type of the current node.
         */
        char *temp = NULL;
        switch (node->type) {
        case AST_CONTROL:
            temp = ast_serialize_control(&node->data.control);
            break;

        case AST_PARAFUNC:
            temp = ast_serialize_parafunc(&node->data.parafunc);
            break;

        case AST_COMPOUND:
            temp = ast_serialize_compound(&node->data.compound);
            break;

        case AST_LITERAL:
            temp = ast_serialize_literal(&node->data.literal);
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
