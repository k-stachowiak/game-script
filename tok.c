#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "error.h"
#include "tok.h"

struct Token *tok_make_token(struct SourceIter *begin, struct SourceIter *end)
{
    struct Token *result = malloc(sizeof(*result));

    if (!result) {
        err_set(ERR_TOK, "Allocation failed.");
        return NULL;
    }

    result->loc = begin->loc;
    result->begin = begin->current;
    result->end = end->current;
    result->next = NULL;
    return result;
}

void tok_free(struct Token *current)
{
    struct Token *next;

    LOG_TRACE_FUNC

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

int tok_list_length(struct Token* current)
{
    int result = 0;

    LOG_TRACE_FUNC

    while (current) {
        ++result;
        current = current->next;
    }
    return result;
}

bool tok_is_spec(struct Token *tok, enum TokenType type)
{
    LOG_TRACE_FUNC
    if (tok->end - tok->begin != 1) {
        return false;
    }

    return *(tok->begin) == type;
}

bool tok_is_open_paren(struct Token *tok)
{
    LOG_TRACE_FUNC
    if (tok->end - tok->begin != 1) {
        return false;
    }

    return *(tok->begin) == TOK_CORE_OPEN ||
           *(tok->begin) == TOK_ARR_OPEN ||
           *(tok->begin) == TOK_TUP_OPEN;
}

bool tok_is_close_paren(struct Token *tok)
{
    LOG_TRACE_FUNC
    if (tok->end - tok->begin != 1) {
        return false;
    }

    return *(tok->begin) == TOK_CORE_CLOSE ||
           *(tok->begin) == TOK_ARR_CLOSE ||
           *(tok->begin) == TOK_TUP_CLOSE;
}

bool tok_paren_match(struct Token *lhs, struct Token *rhs)
{
    LOG_TRACE_FUNC
    if (lhs->end - lhs->begin != 1 ||
        rhs->end - rhs->begin != 1) {
        return false;
    }

    return
        (*(lhs->begin) == TOK_CORE_OPEN && *(rhs->begin) == TOK_CORE_CLOSE) ||
        (*(lhs->begin) == TOK_ARR_OPEN &&  *(rhs->begin) == TOK_ARR_CLOSE) ||
        (*(lhs->begin) == TOK_TUP_OPEN &&  *(rhs->begin) == TOK_TUP_CLOSE);
}

