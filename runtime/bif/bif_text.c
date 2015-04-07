/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdio.h>

#include "eval.h"
#include "memory.h"
#include "error.h"
#include "rt_val.h"
#include "bif.h"
#include "bif_detail.h"

static void bif_text_error_arg(int arg, char *func, char *condition)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL TEXT", eval_location_top());
    err_msg_append(&msg, "Argument %d of _%s_ %s", arg, func, condition);
    err_msg_set(&msg);
}

static void bif_text_error_wc_mismatch(void)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL TEXT", eval_location_top());
    err_msg_append(&msg, "Wildcard type mismatched argument");
    err_msg_set(&msg);
}

static void bif_text_error_wc_unknown(char wildcard)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL TEXT", eval_location_top());
    err_msg_append(&msg, "Wildcard '%c' unknown", wildcard);
    err_msg_set(&msg);
}

static void bif_text_error_args_left(int count)
{
    struct ErrMessage msg;
    err_msg_init_src(&msg, "BIF EVAL TEXT", eval_location_top());
    err_msg_append(&msg, "%d arguments left after printf", count);
    err_msg_set(&msg);
}

static char *bif_text_find_format(char *str)
{
    while (*str != '\0') {
        if (*str == '%') {
            if (*(str + 1) == '%') {
                ++str;
            } else {
                return str;
            }
        }
        ++str;
    }
    return str;
}

static int bif_printf_try_print_arg(struct Runtime *rt, char wc, VAL_LOC_T loc)
{
    enum ValueType type = rt_val_peek_type(rt, loc);

    switch (wc) {
    case 'b':
        if (type != VAL_BOOL) {
            bif_text_error_wc_mismatch();
            return -1;
        }
        return printf("%s", rt_val_peek_bool(rt, loc) ? "true" : "false");

    case 'c':
        if (type != VAL_CHAR) {
            bif_text_error_wc_mismatch();
            return -1;
        }
        return printf("%c", rt_val_peek_char(rt, loc));

    case 'd':
        if (type != VAL_INT) {
            bif_text_error_wc_mismatch();
            return -1;
        }
        return printf("%ld", rt_val_peek_int(rt, loc));

    case 'f':
        if (type != VAL_REAL) {
            bif_text_error_wc_mismatch();
            return -1;
        }
        return printf("%f", rt_val_peek_real(rt, loc));

    case 's':
        if (type != VAL_STRING) {
            bif_text_error_wc_mismatch();
            return -1;
        }
        return printf("%s", rt_val_peek_string(rt, loc));

    default:
        bif_text_error_wc_unknown(wc);
        return -1;
    }
}

static void bif_printf_impl(
        struct Runtime *rt,
        char *str,
        int argc,
        VAL_LOC_T arg_loc)
{
    char *copy, *begin, *end;
    int args_left = argc;
    int len, result = 0;
    bool done = false;

    len = strlen(str);
    copy = mem_malloc(len + 1);
    memcpy(copy, str, len + 1);

    begin = copy;
    while (true) {

        end = bif_text_find_format(begin);
        if (*end == '\0') {
            done = true;
        }

        *end = '\0';
        result += printf("%s", begin);

        if (done) {
            break;
        }

        begin = end + 1; /* skip '%' */
        result += bif_printf_try_print_arg(rt, *begin, arg_loc);
        if (err_state()) {
            goto end;
        }

        arg_loc = rt_val_next_loc(rt, arg_loc);
        ++begin; /* skip wildcard */

        --args_left;
    }

    if (args_left) {
        bif_text_error_args_left(args_left);
    } else {
        rt_val_push_int(rt->stack, result);
    }

end:
    mem_free(copy);
}

static void bif_parse_string_ast(struct AstNode *ast);

static void bif_parse_string_compound(struct AstNode *ast)
{
    exit(1);
}

static void bif_parse_string_literal(struct AstNode *ast)
{
    exit(1);
}

static void bif_parse_string_ast(struct AstNode *ast)
{
    switch (ast->type) {
    case AST_COMPOUND:
        bif_parse_string_compound(&ast->data.compound);
        break;

    case AST_LITERAL:
        bif_parse_string_literal(&ast->data.literal);
        break;

    case AST_DO_BLOCK:
    case AST_BIND:
    case AST_FUNC_CALL:
    case AST_FUNC_DEF:
    case AST_PARAFUNC:
    case AST_REFERENCE:
        bif_text_error_parse("Incorrect string passed to parse");
        break;
    }
}

static void bif_parse_string(char *string)
{
    struct DomNode *dom;
    struct AstNode *ast;

    dom = lex(string);
    if (err_state()) {
        bif_text_error_parse("Failed lexing");
        return;
    }

    ast = parse_list(dom);
    if (err_state()) {
        bif_text_error_parse("Failed AST parsing");
        dom_free(dom);
        return;
    }

    if (ast->next != NULL) {
        bif_text_error_parse("A non-single expression passed for parsing");
        dom_free(dom);
        return;
    }

    dom_free(dom);
    bif_parse_string_ast(ast);
    ast_node_free(ast);
}

void bif_putc(struct Runtime *rt, VAL_LOC_T char_loc)
{
    if (rt_val_peek_type(rt, char_loc) != VAL_CHAR) {
        bif_text_error_arg(1, "putc", "must be a character");
    } else {
        char character = rt_val_peek_char(rt, char_loc);
        putc(character, stdout);
    }
}

void bif_print(struct Runtime *rt, VAL_LOC_T str_loc)
{
    if (rt_val_peek_type(rt, str_loc) != VAL_STRING) {
        bif_text_error_arg(1, "print", "must be a string");
    } else {
        char *string = rt_val_peek_string(rt, str_loc);
        printf("%s", string);
    }
}

void bif_printf(struct Runtime *rt, VAL_LOC_T fmt_loc, VAL_LOC_T args_loc)
{
    char *string;
    int argc;

    if (rt_val_peek_type(rt, fmt_loc) != VAL_STRING) {
        bif_text_error_arg(1, "print", "must be a string");
        return;
    }

    if (rt_val_peek_type(rt, args_loc) != VAL_TUPLE) {
        bif_text_error_arg(2, "print", "must be a tuple");
        return;
    }

    string = rt_val_peek_string(rt, fmt_loc);
    argc = rt_val_cpd_len(rt, args_loc);

    bif_printf_impl(rt, string, argc, rt_val_cpd_first_loc(args_loc));
}

void bif_to_string(struct Runtime *rt, VAL_LOC_T arg_loc)
{
    char *buffer = NULL;
    rt_val_to_string(rt, arg_loc, &buffer);

    if (!buffer) {
        bif_text_error_stringify();
    } else {
        rt_val_push_string(rt->stack, buffer);
        mem_free(buffer);
    }
}

void bif_parse(struct Runtime *rt, VAL_LOC_T arg_loc)
{
    if (rt_val_peek_type(rt, arg_loc) != VAL_STRING) {
        bif_text_error_arg(1, "parse", "must be a string");
        return;
    }

    bif_parse_string(rt_val_peek_string(rt, arg_loc));
}

