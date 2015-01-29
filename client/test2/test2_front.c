#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "dom.h"
#include "parse.h"

#include "test2_detail.h"

static void test_lex(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success)
{
    struct DomNode *node = lex(source);
    tc_record(tc, test_name, !(expect_success ^ (!!node)));
    if (node) {
        dom_free(node);
    }
}

static void test_parse(
        struct TestContext *tc,
        char *source,
        char *test_name,
        bool expect_success)
{
    struct AstNode *node = parse_source(source);
    tc_record(tc, test_name, !(expect_success ^ (!!node)));
    if (node) {
        ast_node_free(node);
    }
}

static void test_parse_literal_string(
        struct TestContext *tc,
        char *source,
        char *expected_value,
        char *test_name)
{
    struct AstNode *node = parse_source(source);
    
    if (!node) {
        tc_record(tc, test_name, false);
        return;
    }

    if (node->type != AST_LITERAL) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }

    if (node->data.literal.type != AST_LIT_STRING) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }
    
    if (strcmp(node->data.literal.data.string, expected_value) != 0) {
        tc_record(tc, test_name, false);
        ast_node_free(node);
        return;
    }

    tc_record(tc, test_name, true);
    ast_node_free(node);
}


#define test_parse_literal(TC, SRC, NAME, EX_ASTTYPE, EX_CTYPE, EX_VALUE) \
    do { \
        struct AstNode *node = parse_source(SRC); \
        if (!node) { \
            tc_record(TC, NAME, false); \
            break; \
        } \
        if (node->type != AST_LITERAL) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        if (node->data.literal.type != EX_ASTTYPE) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        if (*((EX_CTYPE*)(&(node->data.literal.data))) != EX_VALUE) { \
            tc_record(TC, NAME, false); \
            ast_node_free(node); \
            break; \
        } \
        tc_record(TC, NAME, true); \
        ast_node_free(node); \
    } while (0)

static void test_lex_atom(struct TestContext *tc)
{
    test_lex(tc, "\"asdf", "Fail on undelimited double quote", false);
    test_lex(tc, "\'asdf", "Fail on undelimited single quote", false);
    test_lex(tc, "asdf", "Succeed on plain atom", true);
    test_lex(tc, "\"asdf\"", "Succeed on double quote", true);
    test_lex(tc, "\'asdf\'", "Succeed on single quote", true);
}

static void test_lex_compound(struct TestContext *tc)
{
    test_lex(tc, "( ()", "Fail on undelimited homogenous tokens", false);
    test_lex(tc, "( { ( }", "Fail on undelimited heterogenous tokens", false);
    test_lex(tc, "( [] { () } )", "Succeed on properly delimited heterogenous tokens", true);
}

static void test_lex_mixed(struct TestContext *tc)
{
    test_lex(tc, "( atom ( )", "Fail on undelimited complex expression", false);
    test_lex(tc, "this (should () be (lexed) nicely)", "Succeed on well formed complex expression", true);
}

static void test_parse_do(struct TestContext *tc)
{
    test_parse(tc, "(do)", "Fail on empty do block", false);
    test_parse(tc, "[do 1]", "Fail on array do block.", false);
    test_parse(tc, "{do 1}", "Fail on tuple do block.", false);
    test_parse(tc, "(do 1)", "Succeed on simplistic do block", true);
    test_parse(tc, "(do (bind x 2) (+ x x))", "Succeed on non-trivial do block", true);
}

static void test_parse_bind(struct TestContext *tc)
{
    test_parse(tc, "(bind)", "Fail on empty bind block", false);
    test_parse(tc, "(bind x)", "Fail on bind without bound value", false);
    test_parse(tc, "(bind x 1 2)", "Fail on bind with extra element", false);
    test_parse(tc, "[bind x 1]", "Fail on bind in array compound", false);
    test_parse(tc, "{bind x 1}", "Fail on bind in tuple compound", false);
    test_parse(tc, "(bind x 1)", "Succeed on simplistic bind", true);
    test_parse(tc, "(bind x (+ 2 2))", "Succeed on non-trivial bind", true);
}

static void test_parse_iff(struct TestContext *tc)
{
    test_parse(tc, "(if)", "Fail on empty iff block", false);
    test_parse(tc, "(if x)", "Fail on iff with only one argument", false);
    test_parse(tc, "(if x y)", "Fail on iff with only two arguments", false);
    test_parse(tc, "(if x y z s)", "Fail on iff with extra arguments", false);
    test_parse(tc, "[if x y z]", "Fail on iff in array comopund", false);
    test_parse(tc, "{if x y z}", "Fail on iff in tuple comopund", false);
    test_parse(tc, "(if true 1 2)", "Succeed on simplistic iff expression", true);
}

static void test_parse_compound(struct TestContext *tc)
{
    test_parse(tc, "()", "Fail on empty core compound", false);
    test_parse(tc, "(1 2 3)", "Fail on non-empty core compound", false);
    test_parse(tc, "[]", "Succeed on empty array", true);
    test_parse(tc, "[1 2 3]", "Succeed on simple array", true);
    test_parse(tc, "[1 [2 3]]", "Succeed on non-trivial array", true);
    test_parse(tc, "{}", "Succeed on empty tuple", true);
    test_parse(tc, "{1 2 3}", "Succeed on simple tuple", true);
    test_parse(tc, "{1 2 {} 3}", "Succeed on non-trivial tuple", true);
    test_parse(tc, "{1 [2] 3}", "Succeed on mixed compound", true);
}

static void test_parse_func_call(struct TestContext *tc)
{
    test_parse(tc, "(0bad_symbol)", "Fail on incorrect function symbol", false);
    test_parse(tc, "(+ 2 2)", "Succeed on simplistic function call", true);
    test_parse(tc, "(+ (* 2 2) (- 2 2))", "Succeed on non-trivial function call", true);
}

static void test_parse_func_def(struct TestContext *tc)
{
    test_parse(tc, "(func)", "Fail on missing args and body", false);
    test_parse(tc, "(func (x))", "Fail on missing body", false);
    test_parse(tc, "(func [x] (sqrt x))", "Fail on array compound args", false);
    test_parse(tc, "(func {x} (sqrt x))", "Fail on tuple compound args", false);
    test_parse(tc, "(func (x) (sqrt x) (+ x x))", "Fail on extra expression", false);
    test_parse(tc, "(func (x) (sqrt x))", "Succeed on simple function definition", true);
    test_parse(tc, "(func () (sqrt (* 42 42)))", "Succeed on no-arg function definition", true);
}

static void test_parse_literals(struct TestContext *tc)
{
    test_parse_literal(tc, "true", "Boolean true", AST_LIT_BOOL, int, true);
    test_parse_literal(tc, "false", "Boolean false", AST_LIT_BOOL, int, false);

    test_parse_literal_string(tc, "\"simple string\"", "simple string", "Simple string");
    test_parse_literal_string(tc,
            "\"string \\\"with\\\" escaped 'quotes'\"",
            "string \"with\" escaped 'quotes'",
            "Quoted string");

    test_parse_literal(tc, "\'a\'", "Character \'a\'", AST_LIT_CHAR, char, 'a');
    test_parse_literal(tc, "\'\n\'", "Character \'\\n\'", AST_LIT_CHAR, char, '\n');
    test_parse(tc, "\'ab\'", "Fail on too long character literal", false);

    test_parse_literal(tc, "1", "Integer 1", AST_LIT_INT, long, 1);
    test_parse_literal(tc, "+2", "Integer +2", AST_LIT_INT, long, 2);
    test_parse_literal(tc, "-3", "Integer -3", AST_LIT_INT, long, -3);
    test_parse(tc, "42L", "Fail on integer with an alphabetic character", false);

    test_parse_literal(tc, "1.0", "Real 1.0", AST_LIT_REAL, double, 1.0);
    test_parse_literal(tc, "+2.0", "Real +2.0", AST_LIT_REAL, double, 2.0);
    test_parse_literal(tc, "-3.0", "Real -3.0", AST_LIT_REAL, double, -3.0);
    test_parse_literal(tc, "4.", "Real 4.", AST_LIT_REAL, double, 4.0);
    test_parse_literal(tc, ".5", "Real .5", AST_LIT_REAL, double, 0.5);
    test_parse(tc, "6E7", "Fail on real scientific notation", false);
    test_parse(tc, "8e9", "Fail on real scientific notation", false);
}

void test2_front(struct TestContext *tc)
{
    test_lex_atom(tc);
    test_lex_compound(tc);
    test_lex_mixed(tc);
    test_parse_do(tc);
    test_parse_bind(tc);
    test_parse_iff(tc);
    test_parse_compound(tc);
    test_parse_func_call(tc);
    test_parse_func_def(tc);
    test_parse_literals(tc);
}

