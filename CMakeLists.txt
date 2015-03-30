cmake_minimum_required(VERSION 2.8)

project(moon-lang)

include_directories(
    ./ast
    ./client/repl
    ./client/run
    ./client/test
    ./common
    ./front
    ./runtime
    ./runtime/bif
    ./runtime/eval
    .
    )

set(CMAKE_C_FLAGS "-Wall -g -O0")
set(CMAKE_C_COMPILER "gcc")

add_library(lang
    ./front/dom.c
    ./front/lex.c
    ./front/parse.c
    ./front/tok.c
    ./runtime/bif/bif_logic.c
    ./runtime/bif/bif_arythm.c
    ./runtime/bif/bif_array.c
    ./runtime/bif/bif_compare.c
    ./runtime/bif/bif_common.c
    ./runtime/bif/bif_text.c
    ./runtime/eval/eval.c
    ./runtime/eval/eval_fdef.c
    ./runtime/eval/eval_do.c
    ./runtime/eval/eval_parafunc.c
    ./runtime/eval/eval_cpd.c
    ./runtime/eval/eval_fcall.c
	./runtime/eval/eval_bind.c
	./runtime/dbg.c
    ./runtime/symmap.c
    ./runtime/stack.c
    ./runtime/runtime.c
    ./runtime/rt_val.c
    ./ast/ast.c
    ./ast/pattern.c
    ./common/error.c
    ./common/term.c
    ./common/memory.c
    ./common/src_iter.c
	)

add_executable(moon-lang
    ./cli.c
    ./client/run/run.c
    ./client/repl/repl_cmd.c
    ./client/repl/repl.c
    ./client/repl/repl_expr.c
    ./client/test/test.c
    ./client/test/test_helpers.c
    ./client/test/test_front.c
    ./client/test/test_runtime_basic.c
    ./client/test/test_runtime_func.c
    ./client/test/test_runtime_bif.c
    )
target_link_libraries(moon-lang m lang)

add_library(moon ./moon.c)
target_link_libraries(moon m lang)

