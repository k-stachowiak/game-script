/* Copyright (C) 2014 Krzysztof Stachowiak */

#ifndef AST_H
#define AST_H

#include "common.h"

/* Enumerations.
 * =============
 */

enum AstNodeType {
    AST_BIF,
	AST_DO_BLOCK,
    AST_BIND,
	AST_IFF,
    AST_COMPOUND,
    AST_FUNC_CALL,
    AST_FUNC_DEF,
    AST_LITERAL,
    AST_REFERENCE
};

enum AstBifType {
    AST_BIF_UNARY,
    AST_BIF_BINARY,
	AST_BIF_TERNARY
};

enum AstCompoundType {
    AST_CPD_ARRAY,
    AST_CPD_TUPLE
};

enum AstLiteralType {
    AST_LIT_BOOL,
    AST_LIT_STRING,
    AST_LIT_CHAR,
    AST_LIT_INT,
    AST_LIT_REAL
};

/* Partial types.
 * ==============
 */

struct AstCommonFunc {
    char **formal_args;
    struct SourceLocation *arg_locs;
    int arg_count;
};

struct AstNode;
struct Value;
struct Stack;

struct AstBif {
    struct AstCommonFunc func;
    enum AstBifType type;
	void(*u_impl)(struct Stack*, VAL_LOC_T);
	void(*bi_impl)(struct Stack*, VAL_LOC_T, VAL_LOC_T);
	void(*ter_impl)(struct Stack*, VAL_LOC_T, VAL_LOC_T, VAL_LOC_T);
};

struct AstDoBlock {
	struct AstNode *exprs;
};

struct AstBind {
    char *symbol;
    struct AstNode *expr;
};

struct AstIff {
	struct AstNode *test;
	struct AstNode *true_expr;
	struct AstNode *false_expr;
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
    struct AstCommonFunc func;
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
        struct AstBif bif;
		struct AstDoBlock do_block;
        struct AstBind bind;
		struct AstIff iff;
        struct AstCompound compound;
        struct AstFuncCall func_call;
        struct AstFuncDef func_def;
        struct AstLiteral literal;
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
    char *symbol, struct AstNode *expr);

struct AstNode *ast_make_iff(
	struct SourceLocation *loc,
	struct AstNode *test,
	struct AstNode *true_expr,
	struct AstNode *false_expr);

struct AstNode *ast_make_compound(
    struct SourceLocation *loc,
    enum AstCompoundType type,
    struct AstNode *exprs);

struct AstNode *ast_make_func_call(
    struct SourceLocation *loc,
    char *symbol, struct AstNode *args);

struct AstNode *ast_make_func_def(
    struct SourceLocation *loc,
    char **formal_args,
    struct SourceLocation *arg_locs,
    int arg_count,
    struct AstNode *expr);

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
