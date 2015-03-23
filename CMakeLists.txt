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
    .
    )

set(CMAKE_C_FLAGS "-Wall -g -O0 -flto")

add_executable(moon-lang
    ./main.c
    ./front/dom.c
    ./front/lex.c
    ./front/parse.c
    ./front/tok.c
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
    ./runtime/bif_logic.c
    ./runtime/bif_arythm.c
    ./runtime/bif_array.c
    ./runtime/bif_compare.c
    ./runtime/bif_common.c
	./runtime/dbg.c
    ./runtime/symmap.c
    ./runtime/stack.c
    ./runtime/eval.c
    ./runtime/runtime.c
    ./runtime/eval_fdef.c
    ./runtime/eval_do.c
    ./runtime/eval_parafunc.c
    ./runtime/eval_cpd.c
    ./runtime/eval_fcall.c
	./runtime/eval_bind.c
    ./runtime/rt_val.c
    ./ast/ast.c
    ./ast/pattern.c
    ./common/error.c
    ./common/term.c
    ./common/memory.c
    ./common/src_iter.c
    )

target_link_libraries(moon-lang m)

