#include <stdlib.h>

#include "lex.h"
#include "dom.h"

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

static void test2_lex_atom(struct TestContext *tc)
{
    test_lex(tc, "\"asdf", "Fail on undelimited double quote", false);
    test_lex(tc, "\'asdf", "Fail on undelimited single quote", false);
    test_lex(tc, "asdf", "Succeed on plain atom", true);
    test_lex(tc, "\"asdf\"", "Succeed on double quote", true);
    test_lex(tc, "\'asdf\'", "Succeed on double quote", true);
}

static void test2_lex_compound(struct TestContext *tc)
{
    test_lex(tc, "( ()", "Fail on undelimited homogenous tokens", false);
    test_lex(tc, "( { ( }", "Fail on undelimited heterogenous tokens", false);
    test_lex(tc, "( [] { () } )", "Succeed on properly delimited heterogenous tokens", true);
}

static void test2_lex_mixed(struct TestContext *tc)
{
    test_lex(tc, "( atom ( )", "Fail on undelimited complex expression", false);
    test_lex(tc, "this (should () be (lexed) nicely)", "Succeed on well formed complex expression", true);
}

static void test2_parse_do(struct TestContext *tc)
{
    test_parse(tc, "(do)", "Fail on empty do block", false);
    test_parse(tc, "[do 1]", "Fail on array do block.", false);
    test_parse(tc, "{do 1}", "Fail on tuple do block.", false);
    test_parse(tc, "(do 1)", "Succeed on simplistic do block", true);
    test_parse(tc, "(do (bind x 2) (+ x x))", "Succeed on non-trivial do block", true);
}

static void test2_parse_bind(struct TestContext *tc)
{
    test_parse(tc, "(bind)", "Fail on empty bind block", false);
    test_parse(tc, "(bind x)", "Fail on bind without bound value", false);
    test_parse(tc, "(bind x 1 2)", "Fail on bind with extra element", false);
    test_parse(tc, "[bind x 1]", "Fail on bind in array compound", false);
    test_parse(tc, "{bind x 1}", "Fail on bind in tuple compound", false);
    test_parse(tc, "(bind x 1)", "Succeed on simplistic bind", true);
    test_parse(tc, "(bind x (+ 2 2))", "Succeed on non-trivial bind", true);
}

static void test2_parse_iff(struct TestContext *tc)
{
    test_parse(tc, "(if)", "Fail on empty iff block", false);
    test_parse(tc, "(if x)", "Fail on iff with only one argument", false);
    test_parse(tc, "(if x y)", "Fail on iff with only two arguments", false);
    test_parse(tc, "(if x y z s)", "Fail on iff with extra arguments", false);
    test_parse(tc, "[if x y z]", "Fail on iff in array comopund", false);
    test_parse(tc, "{if x y z}", "Fail on iff in tuple comopund", false);
    test_parse(tc, "(if true 1 2)", "Succeed on simplistic iff expression", true);
}

static void test2_parse_compound(struct TestContext *tc)
{
    test_parse(tc, "()", "Fail on empty core compound", false);
    test_parse(tc, "(1 2 3)", "Fail on non-empty core compound", false);
    test_parse(tc, "[]", "Succeed on empty array", true);
    test_parse(tc, "[1 2 3]", "Succeed on non-empty array", true);
    test_parse(tc, "{}", "Succeed on empty tuple", true);
    test_parse(tc, "{1 2 3}", "Succeed on non-empty tuple", true);
}

void test2_front(struct TestContext *tc)
{
    test2_lex_atom(tc);
    test2_lex_compound(tc);
    test2_lex_mixed(tc);
    test2_parse_do(tc);
    test2_parse_bind(tc);
    test2_parse_iff(tc);
    test2_parse_compound(tc);
}

