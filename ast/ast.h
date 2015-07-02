/* Copyright (C) 2014,2015 Krzysztof Stachowiak */

#ifndef AST_H
#define AST_H

#include "src_iter.h"
#include "pattern.h"

/* Enumerations.
 * =============
 */

enum AstNodeType {
    AST_DO_BLOCK,
    AST_BIND,
    AST_COMPOUND,
    AST_FUNC_CALL,
    AST_FUNC_DEF,
    AST_LITERAL,
    AST_PARAFUNC,
    AST_REFERENCE,
};

enum AstCompoundType {
    AST_CPD_ARRAY,
    AST_CPD_TUPLE
};

enum AstLiteralType {
    AST_LIT_UNIT,
    AST_LIT_BOOL,
    AST_LIT_STRING,
    AST_LIT_CHAR,
    AST_LIT_INT,
    AST_LIT_REAL
};

enum AstParafuncType {
    AST_PARAFUNC_IF,
    AST_PARAFUNC_SWITCH,
    AST_PARAFUNC_AND,
    AST_PARAFUNC_OR,
    AST_PARAFUNC_REF,
    AST_PARAFUNC_PEEK,
    AST_PARAFUNC_POKE,
    AST_PARAFUNC_BEGIN,
    AST_PARAFUNC_END,
    AST_PARAFUNC_SUCC
};

/* Partial types.
 * ==============
 */

struct AstNode;
struct Value;
struct Runtime;

struct AstDoBlock {
    struct AstNode *exprs;
};

struct AstBind {
    struct Pattern *pattern;
    struct AstNode *expr;
};

struct AstCompound {
    enum AstCompoundType type;
    struct AstNode *exprs;
};

struct AstFuncCall {
    char *symbol;
    struct AstNode *actual_args;
};

struct AstFuncDef {
    struct Pattern *formal_args;
    struct SourceLocation *arg_locs;
    int arg_count;
    struct AstNode *expr;
};

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

struct AstReference {
    char *symbol;
};

/* Main AST node definition.
 * =========================
 */

struct AstNode {
    /* Base data. */
    enum AstNodeType type;
    struct SourceLocation loc;

    /* Specific data. */
    union {
        struct AstDoBlock do_block;
        struct AstBind bind;
        struct AstCompound compound;
        struct AstFuncCall func_call;
        struct AstFuncDef func_def;
        struct AstLiteral literal;
        struct AstParafunc parafunc;
        struct AstReference reference;
    } data;

    /* Intrusive list. */
    struct AstNode *next;
};

/* Creation.
 * =========
 */

struct AstNode *ast_make_do_block(
    struct SourceLocation *loc,
    struct AstNode* exprs);

struct AstNode *ast_make_bind(
    struct SourceLocation *loc,
    struct Pattern *pattern,
    struct AstNode *expr);

struct AstNode *ast_make_compound(
    struct SourceLocation *loc,
    enum AstCompoundType type,
    struct AstNode *exprs);

struct AstNode *ast_make_func_call(
    struct SourceLocation *loc,
    char *symbol, struct AstNode *args);

struct AstNode *ast_make_func_def(
        struct SourceLocation *loc,
        struct Pattern *formal_args,
        struct SourceLocation *arg_locs,
        int arg_count,
        struct AstNode *expr);

struct AstNode *ast_make_parafunc(
    struct SourceLocation *loc,
    enum AstParafuncType type,
    struct AstNode *args);

struct AstNode *ast_make_literal_unit(struct SourceLocation *loc);
struct AstNode *ast_make_literal_bool(struct SourceLocation *loc, int value);
struct AstNode *ast_make_literal_string(struct SourceLocation *loc, char *value);
struct AstNode *ast_make_literal_character(struct SourceLocation *loc, char value);
struct AstNode *ast_make_literal_int(struct SourceLocation *loc, long value);
struct AstNode *ast_make_literal_real(struct SourceLocation *loc, double value);
struct AstNode *ast_make_reference(struct SourceLocation *loc, char *symbol);

/* Destruction.
 * ============
 */

void ast_node_free(struct AstNode *node);
void ast_node_free_one(struct AstNode *node);

/* Operations.
 * ===========
 */

int ast_list_len(struct AstNode *head);

#endif
