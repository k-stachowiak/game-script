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

void bif_print(struct Runtime *rt, VAL_LOC_T str_loc)
{
    char *string;
    int len;

    if (rt_val_peek_type(rt, str_loc) != VAL_STRING) {
        bif_text_error_arg(1, "print", "must be a string");
        return;
    }

    string = rt_val_peek_string(rt, str_loc);
    len = strlen(string);
    printf("%s", string);
    rt_val_push_int(rt->stack, len);
}

void bif_printf(struct Runtime *rt, VAL_LOC_T fmt_loc, VAL_LOC_T args_loc)
{
    exit(1);
}
