/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef AST_H
#define AST_H

#include "src_iter.h"
#include "pattern.h"

/* Enumerations.
 * =============
 */

enum AstNodeType {
    AST_CONTROL,    /* Program control; special syntax */
    AST_PARAFUNC,   /* Program control; function-like syntax */
    AST_COMPOUND,   /* Arrays and tuples */
    AST_LITERAL     /* Terminal AST nodes */
};

enum AstControlType {
    AST_CTL_DO,
    AST_CTL_BIND,
    AST_CTL_MATCH,
    AST_CTL_FUNC_DEF,
    AST_CTL_FUNC_CALL,
    AST_CTL_REFERENCE
};

enum AstParafuncType {
    /* Flow control */
    AST_PARAFUNC_IF,
    AST_PARAFUNC_WHILE,

    /* Short circuit logic */
    AST_PARAFUNC_AND,
    AST_PARAFUNC_OR,

    /* References(coordinates) manipulation */
    AST_PARAFUNC_REF,
    AST_PARAFUNC_PEEK,
    AST_PARAFUNC_POKE,
    AST_PARAFUNC_BEGIN,
    AST_PARAFUNC_END,
    AST_PARAFUNC_INC,
    AST_PARAFUNC_SUCC
};

enum AstCompoundType {
    AST_CPD_ARRAY,
    AST_CPD_TUPLE
};

enum AstLiteralType {
    AST_LIT_UNIT,
    AST_LIT_BOOL,
    AST_LIT_STRING, /* Syntactical sugar for char arrays */
    AST_LIT_CHAR,
    AST_LIT_INT,
    AST_LIT_REAL
};

/* Partial types.
 * ==============
 */

struct AstNode;

/* Control definition types.
 * -------------------------
 */

struct AstCtlDo {
    struct AstNode *exprs;
};

struct AstCtlBind {
    struct Pattern *pattern;
    struct AstNode *expr;
};

struct AstCtlMatch {
    struct AstNode *expr;
    struct Pattern *keys;
    struct AstNode *values;
};

struct AstCtlFuncDef {
    struct Pattern *formal_args;
    struct SourceLocation *arg_locs;
    int arg_count;
    struct AstNode *expr;
};

struct AstCtlFuncCall {
    struct AstNode *func;
    struct AstNode *actual_args;
};

struct AstCtlReference {
    char *symbol;
};

struct AstControl {
    enum AstControlType type;
    union {
        struct AstCtlDo doo;
        struct AstCtlBind bind;
        struct AstCtlMatch match;
        struct AstCtlFuncDef fdef;
        struct AstCtlFuncCall fcall;
        struct AstCtlReference reference;
    } data;
};

/* Simple types.
 * -------------
 */

struct AstLiteral {
    enum AstLiteralType type;
    union {
        int boolean;
        char *string;
        char character;
        long integer;
        double real;
    } data;
};

struct AstParafunc {
    enum AstParafuncType type;
    struct AstNode *args;
};

struct AstCompound {
    enum AstCompoundType type;
    struct AstNode *exprs;
};


/* Main AST node definition.
 * =========================
 */

struct AstNode {
    enum AstNodeType type;
    struct SourceLocation loc;
    union {
        struct AstControl control;
        struct AstParafunc parafunc;
        struct AstCompound compound;
        struct AstLiteral literal;
    } data;
    struct AstNode *next;
};

/* Creation.
 * =========
 */

struct AstNode *ast_make_ctl_do(
        struct SourceLocation *loc,
        struct AstNode* exprs);

struct AstNode *ast_make_ctl_bind(
        struct SourceLocation *loc,
        struct Pattern *pattern,
        struct AstNode *expr);

struct AstNode *ast_make_ctl_match(
        struct SourceLocation *loc,
        struct AstNode *expr,
        struct Pattern *keys,
        struct AstNode *values);

struct AstNode *ast_make_ctl_fdef(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr);

struct AstNode *ast_make_ctl_fcall(
        struct SourceLocation *loc,
        struct AstNode *func, struct AstNode *args);

struct AstNode *ast_make_ctl_reference(
        struct SourceLocation *loc,
        char *symbol);

struct AstNode *ast_make_parafunc(
        struct SourceLocation *loc,
        enum AstParafuncType type,
        struct AstNode *args);

struct AstNode *ast_make_compound(
        struct SourceLocation *loc,
        enum AstCompoundType type,
        struct AstNode *exprs);

struct AstNode *ast_make_literal_unit(struct SourceLocation *loc);
struct AstNode *ast_make_literal_bool(struct SourceLocation *loc, int value);
struct AstNode *ast_make_literal_string(struct SourceLocation *loc, char *value);
struct AstNode *ast_make_literal_character(struct SourceLocation *loc, char value);
struct AstNode *ast_make_literal_int(struct SourceLocation *loc, long value);
struct AstNode *ast_make_literal_real(struct SourceLocation *loc, double value);

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
