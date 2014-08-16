/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdbool.h>
#include <math.h>

#include "bif.h"

static bool initialized = false;

static struct Location bif_location = { -1, -1 };

static struct Location bif_arg_locations[] = {
    { -1, -1 }, { -1, -1 }
};

static char *bif_arg_names[] = {
    "a", "b"
};

static void bif_init_common_unary(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 1;
    node->data.bif.type = AST_BIF_ARYTHM_UNARY;
}

static void bif_init_common_binary(struct AstNode *node)
{
    node->type = AST_BIF;
    node->loc = bif_location;
    node->data.bif.func.formal_args = bif_arg_names;
    node->data.bif.func.arg_locs = bif_arg_locations;
    node->data.bif.func.arg_count = 2;
    node->data.bif.type = AST_BIF_ARYTHM_BINARY;
}

static long bif_sqrt_int(long x) { return sqrt(x); }
static double bif_sqrt_real(double x) { return sqrt(x); }

static long bif_add_int(long x, long y) { return x + y; }
static long bif_sub_int(long x, long y) { return x - y; }
static long bif_mul_int(long x, long y) { return x * y; }
static long bif_div_int(long x, long y) { return x / y; }
static double bif_add_real(double x, double y) { return x + y; }
static double bif_sub_real(double x, double y) { return x - y; }
static double bif_mul_real(double x, double y) { return x * y; }
static double bif_div_real(double x, double y) { return x / y; }

struct AstNode bif_sqrt;

struct AstNode bif_add;
struct AstNode bif_sub;
struct AstNode bif_mul;
struct AstNode bif_div;

void bif_assure_init(void)
{
    if (initialized) {
        return;
    }

    initialized = true;

    bif_init_common_unary(&bif_sqrt);
    bif_sqrt.data.bif.un_int_impl = bif_sqrt_int;
    bif_sqrt.data.bif.un_real_impl = bif_sqrt_real;

    bif_init_common_binary(&bif_add);
    bif_add.data.bif.bin_int_impl = bif_add_int;
    bif_add.data.bif.bin_real_impl = bif_add_real;

    bif_init_common_binary(&bif_sub);
    bif_sub.data.bif.bin_int_impl = bif_sub_int;
    bif_sub.data.bif.bin_real_impl = bif_sub_real;

    bif_init_common_binary(&bif_mul);
    bif_mul.data.bif.bin_int_impl = bif_mul_int;
    bif_mul.data.bif.bin_real_impl = bif_mul_real;

    bif_init_common_binary(&bif_div);
    bif_div.data.bif.bin_int_impl = bif_div_int;
    bif_div.data.bif.bin_real_impl = bif_div_real;
}
