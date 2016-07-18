/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef AST_H
#define AST_H

#include "src_iter.h"
#include "pattern.h"

/* Enumerations.
 * =============
 */

enum AstNodeType {
    AST_SYMBOL,
    AST_SPECIAL,
    AST_FUNCTION_CALL,
    AST_LITERAL_COMPOUND,
    AST_LITERAL_ATOMIC
};

enum AstSpecialType {
    /* Flow control */
    AST_SPEC_DO,
    AST_SPEC_MATCH,
    AST_SPEC_IF,
    AST_SPEC_WHILE,
    AST_SPEC_FUNC_DEF,

    /* Short circuit logic */
    AST_SPEC_AND,
    AST_SPEC_OR,

    /* Variables manipulation */
    AST_SPEC_BIND,
    AST_SPEC_REF,
    AST_SPEC_PEEK,
    AST_SPEC_POKE,
    AST_SPEC_BEGIN,
    AST_SPEC_END,
    AST_SPEC_INC,
    AST_SPEC_SUCC
};

enum AstLiteralCompoundType {
    AST_LIT_CPD_ARRAY,
    AST_LIT_CPD_TUPLE
};

enum AstLiteralAtomicType {
    AST_LIT_ATOM_UNIT,
    AST_LIT_ATOM_BOOL,
    AST_LIT_ATOM_STRING, /* Syntactical sugar for char arrays */
    AST_LIT_ATOM_CHAR,
    AST_LIT_ATOM_INT,
    AST_LIT_ATOM_REAL
};

/* Partial types.
 * ==============
 */

struct AstNode;

struct AstSymbol {
    char *symbol;
};

struct AstSpecDo {
    struct AstNode *exprs;
};

struct AstSpecMatch {
    struct AstNode *expr;
    struct Pattern *keys;
    struct AstNode *values;
};

struct AstSpecIf {
    struct AstNode *test;
    struct AstNode *true_expr;
    struct AstNode *false_expr;
};

struct AstSpecWhile {
    struct AstNode *test;
    struct AstNode *expr;
};

struct AstSpecFuncDef {
    struct Pattern *formal_args;
    struct SourceLocation *arg_locs;
    int arg_count;
    struct AstNode *expr;
};

struct AstSpecAnd {
    struct AstNode *exprs;
};

struct AstSpecOr{
    struct AstNode *exprs;
};

struct AstSpecBind {
    struct Pattern *pattern;
    struct AstNode *expr;
};

struct AstSpecRef {
    struct AstNode *expr;
};

struct AstSpecPeek {
    struct AstNode *expr;
};

struct AstSpecPoke {
    struct AstNode *reference;
    struct AstNode *value;
};

struct AstSpecBegin {
    struct AstNode *collection;
};

struct AstSpecEnd {
    struct AstNode *collection;
};

struct AstSpecInc {
    struct AstNode *reference;
};

struct AstSpecSucc {
    struct AstNode *reference;
};

struct AstSpecial {
    enum AstSpecialType type;
    union {
        struct AstSpecDo doo;
        struct AstSpecMatch match;
        struct AstSpecIf iff;
        struct AstSpecWhile whilee;
        struct AstSpecFuncDef func_def;
        struct AstSpecAnd andd;
        struct AstSpecOr orr;
        struct AstSpecBind bind;
        struct AstSpecRef ref;
        struct AstSpecPeek peek;
        struct AstSpecPoke poke;
        struct AstSpecBegin begin;
        struct AstSpecEnd end;
        struct AstSpecInc inc;
        struct AstSpecSucc succ;
    } data;
};

struct AstFuncCall {
    struct AstNode *func;
    struct AstNode *actual_args;
};

struct AstLiteralCompound {
    enum AstLiteralCompoundType type;
    struct AstNode *exprs;
};

struct AstLiteralAtomic {
    enum AstLiteralAtomicType type;
    union {
        int boolean;
        char *string;
        char character;
        long integer;
        double real;
    } data;
};


/* Main AST node definition.
 * =========================
 */

struct AstNode {
    enum AstNodeType type;
    struct SourceLocation loc;
    union {
        struct AstSymbol symbol;
        struct AstSpecial special;
        struct AstFuncCall func_call;
        struct AstLiteralCompound literal_compound;
        struct AstLiteralAtomic literal_atomic;
    } data;
    struct AstNode *next;
};

/* Creation.
 * =========
 */

struct AstNode *ast_make_symbol(
        struct SourceLocation *loc,
        char *symbol);

struct AstNode *ast_make_spec_do(
        struct SourceLocation *loc,
        struct AstNode* exprs);

struct AstNode *ast_make_spec_match(
        struct SourceLocation *loc,
        struct AstNode *expr,
        struct Pattern *keys,
        struct AstNode *values);

struct AstNode *ast_make_spec_if(
        struct SourceLocation *loc,
        struct AstNode* test,
        struct AstNode* true_expr,
        struct AstNode* false_expr);

struct AstNode *ast_make_spec_while(
        struct SourceLocation *loc,
        struct AstNode* test,
        struct AstNode* expr);

struct AstNode *ast_make_spec_func_def(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr);

struct AstNode *ast_make_spec_and(
        struct SourceLocation *loc,
        struct AstNode* exprs);

struct AstNode *ast_make_spec_or(
        struct SourceLocation *loc,
        struct AstNode* exprs);

struct AstNode *ast_make_spec_bind(
        struct SourceLocation *loc,
        struct Pattern *pattern,
        struct AstNode *expr);

struct AstNode *ast_make_spec_ref(
        struct SourceLocation *loc,
        struct AstNode* expr);

struct AstNode *ast_make_spec_peek(
        struct SourceLocation *loc,
        struct AstNode* expr);

struct AstNode *ast_make_spec_poke(
        struct SourceLocation *loc,
        struct AstNode* reference,
        struct AstNode* value);

struct AstNode *ast_make_spec_begin(
        struct SourceLocation *loc,
        struct AstNode* collection);

struct AstNode *ast_make_spec_end(
        struct SourceLocation *loc,
        struct AstNode* collection);

struct AstNode *ast_make_spec_inc(
        struct SourceLocation *loc,
        struct AstNode* reference);

struct AstNode *ast_make_spec_succ(
        struct SourceLocation *loc,
        struct AstNode* reference);

struct AstNode *ast_make_func_call(
        struct SourceLocation *loc,
        struct AstNode *func,
        struct AstNode *args);

struct AstNode *ast_make_literal_compound(
        struct SourceLocation *loc,
        enum AstLiteralCompoundType type,
        struct AstNode *exprs);

struct AstNode *ast_make_literal_atomic_unit(struct SourceLocation *loc);
struct AstNode *ast_make_literal_atomic_bool(struct SourceLocation *loc, int value);
struct AstNode *ast_make_literal_atomic_string(struct SourceLocation *loc, char *value);
struct AstNode *ast_make_literal_atomic_character(struct SourceLocation *loc, char value);
struct AstNode *ast_make_literal_atomic_int(struct SourceLocation *loc, long value);
struct AstNode *ast_make_literal_atomic_real(struct SourceLocation *loc, double value);

/* Destruction.
 * ============
 */

void ast_node_free(struct AstNode *node);
void ast_node_free_one(struct AstNode *node);

/* Serialization.
 * ==============
 */

char *ast_serialize(struct AstNode *node);

/* Introspection.
 * ==============
 */

int ast_list_len(struct AstNode *head);

#endif
