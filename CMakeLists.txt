cmake_minimum_required(VERSION 2.8)

project(moon-lang)

include_directories(
    ./ast
    ./common
    ./front
    ./runtime
    ./runtime/bif
    ./runtime/eval
    ./LIB
    .
    )

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-Wall -g -O0 -std=c11")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -g -O0 -std=c++11")

add_library(lang OBJECT
    ./front/dom.c
    ./front/lex.c
    ./front/parse.c
    ./front/tok.c
    ./runtime/bif/bif_logic.c
    ./runtime/bif/bif_arythm.c
    ./runtime/bif/bif_compound.c
    ./runtime/bif/bif_compare.c
    ./runtime/bif/bif_common.c
    ./runtime/bif/bif_random.c
    ./runtime/bif/bif_text.c
    ./runtime/bif/bif_type.c
    ./runtime/eval/eval.c
    ./runtime/eval/eval_fdef.c
    ./runtime/eval/eval_do.c
    ./runtime/eval/eval_parafunc.c
    ./runtime/eval/eval_cpd.c
    ./runtime/eval/eval_fcall.c
    ./runtime/eval/eval_bind.c
	./runtime/cpprand.cpp
    ./runtime/dbg.c
    ./runtime/symmap.c
    ./runtime/stack.c
    ./runtime/runtime.c
    ./runtime/rt_val.c
    ./runtime/rt_val_peek.c
    ./runtime/rt_val_poke.c
    ./runtime/rt_val_push.c
    ./ast/ast.c
    ./ast/pattern.c
    ./common/error.c
    ./common/term.c
    ./common/memory.c
    ./common/src_iter.c
    ./common/timer_stack.c
    )

add_executable(mntest $<TARGET_OBJECTS:lang>
    ./TEST/test.c
    ./TEST/test_front.c
    ./TEST/test_helpers.c
    ./TEST/test_runtime_basic.c
    ./TEST/test_runtime_bif.c
    ./TEST/test_runtime_func.c
    ./TEST/test_runtime_parafunc.c
    ./TEST/test_runtime_scope.c
    ./TEST/test_cci.c
    )
target_link_libraries(mntest m stdc++)

add_library(moon STATIC $<TARGET_OBJECTS:lang>
	./LIB/clif.c
	./LIB/mndetail.c
    ./LIB/moon.c
    )
target_link_libraries(moon m stdc++)

add_executable(mnrepl ./REPL/main.c)
target_link_libraries(mnrepl moon)

