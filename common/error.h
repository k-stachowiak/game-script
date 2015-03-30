/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "memory.h"

struct SourceLocation;

#define ERR_TEM_BUFFER_SIZE 4096

struct ErrMessage {
    char *text;
};

void err_reset(void);
bool err_state(void);
char *err_msg(void);

void err_msg_init(struct ErrMessage *msg, char *module);
void err_msg_init_src(struct ErrMessage *msg, char *module, struct SourceLocation *loc);
void err_msg_set(struct ErrMessage *msg);

#define err_msg_append(MSG, FORMAT, ...) \
    do { \
        char buffer[ERR_TEM_BUFFER_SIZE]; \
        int old_len, new_len; \
        char *new_text; \
        new_len = sprintf(buffer, " " FORMAT, ##__VA_ARGS__); \
        if (new_len >= (ERR_TEM_BUFFER_SIZE) - 1) { \
            LOG_ERROR("Memory corruption while building error string."); \
            exit(2); \
        } \
        old_len = strlen((MSG)->text); \
        new_text = mem_malloc(old_len + new_len + 1); \
        memcpy(new_text, (MSG)->text, old_len); \
        memcpy(new_text + old_len, buffer, new_len + 1); \
        mem_free((MSG)->text); \
        (MSG)->text = new_text; \
    } while(0)

#endif
