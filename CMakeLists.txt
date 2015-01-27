cmake_minimum_required(VERSION 2.8)

project(moon-lang)

include_directories(
    ./ast
    ./bif
    ./front
    ./repl
    ./test
    ./itpr
    ./run
    ./runtime
    ./common
    .
    )

set(CMAKE_C_FLAGS "-Wall -g -O0")

add_executable(moon-lang
    ./run/run.c
    ./main.c
    ./front/dom.c
    ./front/lex.c
    ./front/parse.c
    ./front/tok.c
    ./bif/bif_logic.c
    ./bif/bif_arythm.c
    ./bif/bif_array.c
    ./bif/bif_compare.c
    ./bif/bif_common.c
    ./runtime/eval_cpd.c
    ./runtime/eval_fcall.c
    ./runtime/symmap.c
    ./runtime/stack.c
    ./runtime/eval.c
    ./runtime/runtime.c
    ./runtime/eval_fdef.c
    ./runtime/rt_val.c
    ./ast/ast.c
    ./repl/repl_cmd.c
    ./repl/repl.c
    ./repl/repl_expr.c
    ./common/error.c
    ./common/term.c
    ./common/memory.c
    ./common/src_iter.c
    ./test/test_regr.c
    ./test/test_runtime.c
    ./test/test_parse.c
    ./test/test.c
    )

target_link_libraries(moon-lang m)

