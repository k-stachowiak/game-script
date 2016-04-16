/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef AST_H
#define AST_H

#include "src_iter.h"
#include "pattern.h"

/* Enumerations.
 * =============
 */

enum AstNodeType {
    AST_CONTROL,
    AST_SPECIAL,
    AST_LITERAL_COMPOUND,
    AST_LITERAL_ATOMIC
};

enum AstControlType {
    AST_CTL_DO,
    AST_CTL_BIND,
    AST_CTL_MATCH,
    AST_CTL_FUNC_DEF,
    AST_CTL_FUNC_CALL,
    AST_CTL_REFERENCE
};

enum AstSpecialType {
    /* Flow control */
    AST_SPECIAL_IF,
    AST_SPECIAL_WHILE,

    /* Short circuit logic */
    AST_SPECIAL_AND,
    AST_SPECIAL_OR,

    /* References(coordinates) manipulation */
    AST_SPECIAL_REF,
    AST_SPECIAL_PEEK,
    AST_SPECIAL_POKE,
    AST_SPECIAL_BEGIN,
    AST_SPECIAL_END,
    AST_SPECIAL_INC,
    AST_SPECIAL_SUCC
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

struct AstSpecial {
    enum AstSpecialType type;
    struct AstNode *args;
};

struct AstLiteralCompound {
    enum AstLiteralCompoundType type;
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
        struct AstSpecial special;
        struct AstLiteralCompound literal_compound;
        struct AstLiteralAtomic literal_atomic;
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

struct AstNode *ast_make_special(
        struct SourceLocation *loc,
        enum AstSpecialType type,
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
