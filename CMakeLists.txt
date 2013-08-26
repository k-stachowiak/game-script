cmake_minimum_required (VERSION 2.8)
project (moon-script)

if (CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-g -o0 -Wall -Werror -std=c++11 -I..")
endif ()

# libraries.
# ----------

find_package (UnitTest++ REQUIRED)

if (UNITTEST++_FOUND)
    include_directories (${UNITTEST++_INCLUDE_DIR})
endif ()

# Intermediate targets
# --------------------

add_library (common OBJECT
    common/expr.cpp
    common/log.cpp
    common/types.cpp)

add_library (parse OBJECT 
    parse/dom_build.cpp
    parse/tokenize.cpp)

add_library (interpret OBJECT
    interpret/env_bif.cpp
    interpret/env_build.cpp
    interpret/env.cpp)

# Unit tests.
# -----------

add_executable (common_test
    common/expr_test.cpp
    common/common_test.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:parse>
    $<TARGET_OBJECTS:interpret>)

target_link_libraries (common_test ${UNITTEST++_LIBRARY})
add_custom_command (TARGET common_test POST_BUILD COMMAND common_test)

add_executable (parse_test
    parse/dom_build_test.cpp
    parse/tokenize_test.cpp
    parse/parse_test.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:parse>
    $<TARGET_OBJECTS:interpret>)

target_link_libraries (parse_test ${UNITTEST++_LIBRARY})
add_custom_command (TARGET parse_test POST_BUILD COMMAND parse_test)

add_executable (interpret_test
    interpret/env_bif_test.cpp
    interpret/env_build_test.cpp
    interpret/interpret_test.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:parse>
    $<TARGET_OBJECTS:interpret>)

target_link_libraries (interpret_test ${UNITTEST++_LIBRARY})
add_custom_command (TARGET interpret_test POST_BUILD COMMAND interpret_test)

