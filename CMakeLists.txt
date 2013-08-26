cmake_minimum_required (VERSION 2.8)
project (moon-script)

if (CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-g -o0 -Wall -Werror -std=c++11")
endif ()

# libraries.
# ----------

find_package (UnitTest++ REQUIRED)

if (UNITTEST++_FOUND)
    include_directories (${UNITTEST++_INCLUDE_DIR})
endif ()

# Unit tests.
# -----------

add_executable (test
    env_build.cpp
    test.cpp
    env_test.cpp
    env_bif_test.cpp
    dom_build_test.cpp
    env_build_test.cpp
    tokenize.cpp
    tokenize_test.cpp
    dom_build.cpp
    env.cpp
    env_test.cpp
    env_bif.cpp
    expr.cpp
    expr_test.cpp
    types.cpp
    log.cpp
)

target_link_libraries (test ${UNITTEST++_LIBRARY})

add_custom_command (TARGET test POST_BUILD COMMAND test)

