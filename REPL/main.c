/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "log.h"
#include "term.h"
#include "memory.h"
#include "moon.h"
#include "timer_stack.h"

static bool quit_request = false;
static bool debug_state = false;
static char *file_to_load = NULL;

static char *stdfilename = "std.mn";
static char *prompt = ": ";
static char *response_prefix = "> ";
static char *time_prefix = ". ";
static char *banner =
    "Moon language REPL\n"
    "Copyright (C) 2014-2015 Krzysztof Stachowiak\n";

static struct MoonContext *ctx = NULL;

static struct MoonValue *make_error(bool value, char *message)
{
    struct MoonValue *result = mem_malloc(sizeof(*result));
    struct MoonValue *result_val = mem_malloc(sizeof(*result));
    struct MoonValue *result_msg = mem_malloc(sizeof(*result));

    result_val->type = MN_BOOL;
    result_val->data.boolean = value;
    result_val->next = result_msg;

    result_msg->type = MN_STRING;
    result_msg->data.string = mem_malloc(strlen(message) + 1);
    memcpy(result_msg->data.string, message, strlen(message) + 1);
    result_msg->next = NULL;

    result->type = MN_TUPLE;
    result->data.compound = result_val;
    result->next = NULL;

    return result;
}

static struct MoonValue *make_unit(void)
{
    struct MoonValue *result = mem_malloc(sizeof(*result));
    result->type = MN_UNIT;
    result->next = NULL;
    return result;
}

struct MoonValue *repl_clif_dbg(struct MoonValue *args)
{
    (void)args;

    if (debug_state) {
        debug_state = false;
        mn_set_debugger(ctx, debug_state);
        printf("Debugger disabled\n");

    } else {
        debug_state = true;
        mn_set_debugger(ctx, debug_state);
        printf("Debugger enabled\n");
    }

    return make_unit();
}

struct MoonValue *repl_clif_quit(struct MoonValue *args)
{
    (void) args;

    quit_request = true;
    return make_unit();
}

struct MoonValue *repl_clif_load(struct MoonValue *args)
{
    char *str;

    if (args->type != MN_STRING) {
        return make_error(false, "load: Argument not string");
    }

    str = args->data.string;
    file_to_load = mem_malloc(strlen(str) + 1);
    memcpy(file_to_load, str, strlen(str) + 1);
    return make_error(true, "No issue");
}

static char *repl_read(void)
{
    bool eof_flag = false;
    char *line;

    printf("%s", prompt);
    line = my_getline(&eof_flag);

    if (eof_flag) {
        mem_free(line);
        return NULL;
    }

    return line;
}

static struct MoonValue *repl_evaluate(char *line)
{
    struct MoonValue *result = mn_exec_command(ctx, line);
    if (file_to_load != NULL) {
        if (!mn_exec_file(ctx, file_to_load)) {
            mn_dispose(result);
            result = make_error(false, "load: Failed loading file");
        }
        mem_free(file_to_load);
        file_to_load = NULL;
    }
    return result;
}

static void repl_print(struct MoonValue *value)
{
    if (!value) {
        printf("unit");
    }

    while (value) {
        switch (value->type) {
        case MN_BOOL:
            printf("%s", value->data.boolean ? "true" : "false");
            break;
        case MN_CHAR:
            printf("%c", value->data.character);
            break;
        case MN_INT:
            printf("%" PRId64, value->data.integer);
            break;
        case MN_REAL:
            printf("%f", value->data.real);
            break;
        case MN_STRING:
            printf("\"%s\"", value->data.string);
            break;
        case MN_ARRAY:
            printf("[");
            repl_print(value->data.compound);
            printf("]");
            break;
        case MN_TUPLE:
            printf("{");
            repl_print(value->data.compound);
            printf("}");
            break;
        case MN_FUNCTION:
            printf("function");
            break;
        case MN_REFERENCE:
            printf("reference");
            break;
        case MN_UNIT:
            printf("unit");
            break;
        }

        if ((value = value->next)) {
            printf(" ");
        }
    }
}

int main()
{
    printf("%s", banner);

    ctx = mn_create();

    if (!mn_exec_file(ctx, stdfilename)) {
        char *error_message = (char*)mn_error_message();
        printf("Error while reading standard library\n");
        printf("%s", error_message);
        mem_free(error_message);
        mn_destroy(ctx);
        return 1;
    }

    if (!mn_register_clif(ctx, "debug", 0, repl_clif_dbg) ||
        !mn_register_clif(ctx, "quit", 0, repl_clif_quit) ||
        !mn_register_clif(ctx, "load", 1, repl_clif_load)) {
            char *error_message = (char*)mn_error_message();
            printf("Error while setting up REPL environment\n");
            printf("%s", error_message);
            mem_free(error_message);
            mn_destroy(ctx);
            return 1;
    }

    while (!quit_request) {

        char *line;
        struct MoonValue *value;

        if (!(line = repl_read())) {
            printf("Null line read\n");
            break;
        }

        ts_start();
        value = repl_evaluate(line);

        if (mn_error_state()) {
            char *error_message = (char*)mn_error_message();
            printf("%s", error_message);
            mem_free(error_message);

        } else {
            long long time = ts_ustop();
            printf("%s", response_prefix);
            repl_print(value);
            printf("\n%s%lld.%lldms\n\n", time_prefix, time / 1000, time % 1000);
            mn_dispose(value);
        }

        mem_free(line);
    }

    ts_deinit();
    mn_destroy(ctx);
    return 0;
}

