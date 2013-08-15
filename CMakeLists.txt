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

add_library (environment environment.cpp)

# Unit tests.
# -----------

add_executable (test test.cpp
        tokenize_test.cpp tokenize.cpp
        dom_build_test.cpp dom_build.cpp
        ast_build_test.cpp ast_build.cpp ast.cpp ast_build_test.cpp ast_build.cpp ast.cpp)

target_link_libraries (test environment ${UNITTEST++_LIBRARY})
add_custom_command (TARGET test POST_BUILD COMMAND test)
