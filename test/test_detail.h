/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdbool.h>

#include "runtime.h"

typedef bool (*TestFunction)(void);
typedef bool (*RuntimeTestFunction)(struct Runtime *);

bool test_source_eval(struct Runtime *rt, char *source, VAL_LOC_T *locs);

bool test_lexer();
bool test_parser();

bool test_regression_real_in_array(struct Runtime *rt);

bool test_runtime_sanity(struct Runtime *rt);
bool test_runtime_free_on_fail(struct Runtime *rt);
bool test_array_homo(struct Runtime *rt);
bool test_bif_cons_homo(struct Runtime *rt);
bool test_bif_cat_homo(struct Runtime *rt);
bool test_local_scope(struct Runtime *rt);
bool test_simple_algorithm(struct Runtime *rt);
bool test_array_lookup(struct Runtime *rt);
bool test_function_object(struct Runtime *rt);
