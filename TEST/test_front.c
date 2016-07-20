
/* Copyright (C) 2015 Krzysztof Stachowiak */

#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"

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
    test_lex(tc, "(])", "Fail on unopened compound", false);
    test_lex(tc, "( [] { () } )", "Succeed on properly delimited heterogenous tokens", true);
}

static void test_lex_mixed(struct TestContext *tc)
{
    test_lex(tc, "( atom ( )", "Fail on undelimited complex expression", false);
    test_lex(tc, "this (should () be (lexed) nicely)", "Succeed on well formed complex expression", true);
}

static void test_lex_comments(struct TestContext *tc)
{
    char *source =
        "#start with a comment\n"
        "(bind # end line with a comment\n"
        "    #interleave with a comment   \n"
        "x 0)\n"
        "(bind y 42) # comment at the end of the source\n"
        "(bind z (func ()\n"
        "    4 # Comment before end of function\n"
        "))";

    test_lex(tc, source, "Succeed on a code interleaved with comments 1.", true);
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
    /* Alias binds */
    test_parse(tc, "(bind () 1)", "Fail on binding to an empty compound", false);
    test_parse(tc, "(bind)", "Fail on empty bind block", false);
    test_parse(tc, "(bind x)", "Fail on bind without bound value", false);
    test_parse(tc, "(bind x 1 2)", "Fail on bind with extra element", false);
    test_parse(tc, "[bind x 1]", "Fail on bind in array compound", false);
    test_parse(tc, "{bind x 1}", "Fail on bind in tuple compound", false);
    test_parse(tc, "(bind x 1)", "Succeed on simplistic bind", true);
    test_parse(tc, "(bind x (+ 2 2))", "Succeed on non-trivial bind", true);

    /* Compound binds */
    test_parse(tc, "(bind { x y } { 2 3 })", "Succeed on compound bind", true);
    test_parse(tc, "(bind [ x { y z } ] [ 1 { 2 3 } ])", "Succeed on recursively compound bind", true);
}

static void test_parse_match(struct TestContext *tc)
{
    test_parse(tc, "[match]", "Fail on match in array", false);
    test_parse(tc, "{match}", "Fail on match in tuple", false);
    test_parse(tc, "(match)", "Fail on empty match block", false);
    test_parse(tc, "(match x)", "Fail on missing cases", false);
    test_parse(tc, "(match x [])", "Fail on missing array as case", false);
    test_parse(tc, "(match x {})", "Fail on missing tuple as case", false);
    test_parse(tc, "(match x ())", "Fail on empty case", false);
    test_parse(tc, "(match x (y))", "Fail on missing case value", false);

    test_parse(tc, "(match x (y z))", "Succeed on simple match", true);
    test_parse(tc, "(match x (y z) (s t))", "Succeed on multiple simple matches", true);

    test_parse(tc, "(match x ([y z] a))", "Succeed on compound match", true);
    test_parse(tc, "(match x ([y z] a) ([s t] b))", "Succeed on multiple compund matches", true);
}

static void test_parse_compound(struct TestContext *tc)
{
    test_parse(tc, "()", "Fail on empty core compound", false);
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
    test_parse_literal(tc, "true", "Boolean true", AST_LIT_ATOM_BOOL, int, true);
    test_parse_literal(tc, "false", "Boolean false", AST_LIT_ATOM_BOOL, int, false);

    test_parse_literal_string(tc, "\"simple string\"", "Simple string");
    test_parse_literal_string(tc,
            "\"string \\\"with\\\" escaped 'quotes'\"",
            "Quoted string");

    test_parse_literal(tc, "\'a\'", "Character \'a\'", AST_LIT_ATOM_CHAR, char, 'a');
    test_parse_literal(tc, "\'\n\'", "Character \'\\n\'", AST_LIT_ATOM_CHAR, char, '\n');
    test_parse(tc, "\'ab\'", "Fail on too long character literal", false);

    test_parse_literal(tc, "1", "Integer 1", AST_LIT_ATOM_INT, long, 1);
    test_parse_literal(tc, "+2", "Integer +2", AST_LIT_ATOM_INT, long, 2);
    test_parse_literal(tc, "-3", "Integer -3", AST_LIT_ATOM_INT, long, -3);
    test_parse(tc, "42L", "Fail on integer with an alphabetic character", false);

    test_parse_literal(tc, "1.0", "Real 1.0", AST_LIT_ATOM_REAL, double, 1.0);
    test_parse_literal(tc, "+2.0", "Real +2.0", AST_LIT_ATOM_REAL, double, 2.0);
    test_parse_literal(tc, "-3.0", "Real -3.0", AST_LIT_ATOM_REAL, double, -3.0);
    test_parse_literal(tc, "4.", "Real 4.", AST_LIT_ATOM_REAL, double, 4.0);
    test_parse_literal(tc, ".5", "Real .5", AST_LIT_ATOM_REAL, double, 0.5);
    test_parse(tc, "6E7", "Fail on real scientific notation", false);
    test_parse(tc, "8e9", "Fail on real scientific notation", false);
}

void test_front(struct TestContext *tc)
{
    test_lex_atom(tc);
    test_lex_compound(tc);
    test_lex_mixed(tc);
    test_lex_comments(tc);
    test_parse_do(tc);
    test_parse_bind(tc);
    test_parse_match(tc);
    test_parse_compound(tc);
    test_parse_func_call(tc);
    test_parse_func_def(tc);
    test_parse_literals(tc);
}
