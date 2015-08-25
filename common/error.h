/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "collection.h"
#include "memory.h"
#include "strbuild.h"
#include "src_iter.h"

struct ErrFrame {
    char *module;
    char *message;
    struct SourceLocation src_loc;
    struct ErrFrame *next;
};

extern struct ErrFrame *err_stack;
extern struct ErrFrame *err_stack_end;

void err_reset(void);
bool err_state(void);
char *err_msg(void);
void err_report(void);

#define err_push_src(MODULE, SRC_LOC, FORMAT, ...) \
    do { \
        struct ErrFrame *_frame_ = mem_malloc(sizeof(*_frame_)); \
        _frame_->module = (MODULE); \
        _frame_->message = NULL; \
        _frame_->src_loc = (SRC_LOC); \
        _frame_->next = NULL; \
        str_append(_frame_->message, FORMAT, ##__VA_ARGS__); \
        LIST_APPEND(_frame_, &err_stack, &err_stack_end); \
    } while(0)

#define err_push(MODULE, FORMAT, ...) err_push_src(MODULE, src_loc_virtual(), FORMAT, ##__VA_ARGS__)

#endif
