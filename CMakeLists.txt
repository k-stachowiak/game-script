cmake_minimum_required(VERSION 2.8)

project(moon-lang)

include_directories(
    ./common
    ./front
    ./LIB
    ./runtime
    ./runtime/bif
    ./runtime/eval
    ./util
    .
    )

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

#set(CMAKE_C_FLAGS "-Wall -Wextra -g -O0 -std=c11")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -O0 -std=c++11")
set(CMAKE_C_FLAGS "-Wall -Wextra -O2 -std=c11")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2 -std=c++11")

add_library(lang OBJECT
        ./common/pattern.c
        ./common/ast.c
        ./common/src_iter.c
        ./common/error.c
        ./common/api_value.c
        ./common/test.c
        ./front/lex.c
        ./front/parse.c
        ./front/tok.c
        ./front/dom.c
        ./util/term.c
        ./util/timer_stack.c
        ./util/memory.c
        ./runtime/cpprand.cpp
        ./runtime/symmap.c
        ./runtime/stack.c
        ./runtime/rt_val.c
        ./runtime/rt_val_poke.c
        ./runtime/eval/eval.c
        ./runtime/eval/eval_parafunc.c
        ./runtime/eval/eval_fcall.c
        ./runtime/eval/eval_cpd.c
        ./runtime/eval/eval_fdef.c
        ./runtime/eval/eval_bind.c
        ./runtime/bif/bif_compound.c
        ./runtime/bif/bif_type.c
        ./runtime/bif/bif_random.c
        ./runtime/bif/bif_compare.c
        ./runtime/bif/bif_arythm.c
        ./runtime/bif/bif_text.c
        ./runtime/bif/bif_common.c
        ./runtime/bif/bif_logic.c
        ./runtime/dbg.c
        ./runtime/runtime.c
        ./runtime/rt_val_peek.c
        ./runtime/rt_val_push.c
    )

add_executable(mntest $<TARGET_OBJECTS:lang>
    ./TEST/test_app.c
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
    ./common/api_value.c
    ./LIB/moon.c
    )
target_link_libraries(moon m stdc++)

add_executable(mnrepl ./REPL/main.c)
target_link_libraries(mnrepl moon)

