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

static void bif_text_error_ws_unknown(char wildcard)
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

static char *bif_text_find(char *str, char value)
{
    while (*str != '\0' && *str != value) {
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
        bif_text_error_ws_unknown(wc);
        return -1;
    }
}

static void bif_printf_impl(
        struct Runtime *rt,
        char *str,
        int argc,
        VAL_LOC_T arg_loc)
{
    char *copy, *end;
    int args_left = argc;
    int len, result = 0;
    bool done = false;

    len = strlen(str);
    copy = mem_malloc(len + 1);
    memcpy(copy, str, len + 1);

    while (true) {
        end = bif_text_find(copy, '%');
        if (*end == '\0') {
            done = true;
        }
        *end = '\0';
        result += printf("%s", copy);
        if (done) {
            break;
        }

        ++copy; /* skip '%' */
        result += bif_printf_try_print_arg(rt, *copy, arg_loc);
        if (err_state()) {
            goto end;
        }
        arg_loc = rt_val_next_loc(rt, arg_loc);
        ++copy; /* skip wildcard */

        --args_left;
    }

    if (args_left) {
        bif_text_error_args_left(args_left);
        goto end;
    }

    rt_val_push_int(rt->stack, result);

end:
    mem_free(copy);
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
    char *string;
    int len;

    if (rt_val_peek_type(rt, str_loc) != VAL_STRING) {
        bif_text_error_arg(1, "print", "must be a string");
    } else {
        string = rt_val_peek_string(rt, str_loc);
        len = strlen(string);
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
