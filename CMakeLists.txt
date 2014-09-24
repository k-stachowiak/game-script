cmake_minimum_required(VERSION 2.8)

project(moon-lang)

include_directories(
    ./ast
    ./bif
    ./front
    ./repl
    ./test
    ./itpr
    ./runtime
    .
    )

set(CMAKE_C_FLAGS "-Wall -Werror -g -O0")

add_executable(moon-lang
    ./ast/ast_parse.c
    ./ast/ast.c
    ./bif/bif_arythm.c
    ./bif/bif_array.c
    ./bif/bif_common.c
    ./bif/bif_compare.c
    ./bif/bif_logic.c
    ./front/parse.c
    ./front/lex.c
    ./front/dom.c
    ./front/tok.c
    ./repl/repl_expr.c
    ./repl/repl_cmd.c
    ./repl/repl.c
    ./test/test.c
    ./runtime/eval_fdef.c
    ./runtime/stack.c
    ./runtime/eval_fcall.c
    ./runtime/value.c
    ./runtime/eval.c
    ./runtime/runtime.c
    ./runtime/symmap.c
    ./common.c
    ./error.c
    ./main.c
    )

target_link_libraries(moon-lang m)

