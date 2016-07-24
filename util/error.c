/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "src_iter.h"
#include "memory.h"
#include "error.h"

struct ErrFrame *err_stack = NULL;
struct ErrFrame *err_stack_end = NULL;

void err_reset(void)
{
    err_stack_end = NULL;
    while (err_stack) {
        struct ErrFrame *next = err_stack->next;
        mem_free(err_stack->message);
        mem_free(err_stack);
        err_stack = next;
    }
}

bool err_state(void)
{
    return (bool)err_stack;
}

char *err_msg(void)
{
    char *result = NULL;
    struct ErrFrame *frame = err_stack;
    int i = 0;

    str_append(result, "Error:\n");
    while (frame) {
        char *message_line = NULL;
        str_append(message_line, "%d: [%s] ", i++, frame->module);
        if (frame->src_loc) {
            str_append(
        message_line,
        "(%d,%d) ",
        frame->src_loc->line,
        frame->src_loc->column);
        }
        str_append(message_line, ": %s\n", frame->message);
        str_append(result, "\t%s", message_line);
        mem_free(message_line);
        frame = frame->next;
    }
    return result;
}

void err_report(void)
{
    if (err_state()) {
        char *msg = err_msg();
        printf("%s", msg);
        mem_free(msg);
    } else {
        printf("No error\n");
    }
}

