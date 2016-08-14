/* Copyright (C) 2014-2016 Krzysztof Stachowiak */

#ifndef MOON_H
#define MOON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MoonValueType {
    MN_BOOL,
    MN_CHAR,
    MN_INT,
    MN_REAL,
    MN_STRING,
    MN_ARRAY,
    MN_TUPLE,
    MN_FUNCTION,
    MN_REFERENCE,
    MN_UNIT
};

struct MoonValue {
    enum MoonValueType type;
    union {
        bool boolean;
        char character;
        int64_t integer;
        double real;
        char *string;
        struct MoonValue *compound;
        int64_t pointer;
    } data;
    struct MoonValue *next;
};

typedef struct MoonValue* (*ClifHandler)(struct MoonValue *args);

struct MoonContext;

struct MoonContext *mn_create(void);
void mn_destroy(struct MoonContext *ctx);

void mn_set_debugger(struct MoonContext *ctx, bool state);
bool mn_register_clif(struct MoonContext *ctx, const char *symbol, int arity, ClifHandler handler);
bool mn_exec_file(struct MoonContext *ctx, const char *filename);
struct MoonValue *mn_exec_command(struct MoonContext *ctx, const char *source);
void mn_dispose(struct MoonValue* value);

bool mn_error_state(void);
const char *mn_error_message(void);

#ifdef __cplusplus
}
#endif

#endif
