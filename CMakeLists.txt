cmake_minimum_required(VERSION 3.12)

project(baleen)

add_library(baleen)

target_sources(baleen
  PRIVATE
    src/brilprogram.cpp
    src/brilobject.cpp
    src/stringtable.cpp
    src/brilop.cpp
    src/brilprimitive.cpp
    src/briltype.cpp
    src/brilbasicblock.cpp
    src/baleen_utils.cpp
    src/brilfunction.cpp
    src/lvn.cpp
  PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS include
    FILES
      include/brilprogram.h
      include/brilobject.h
      include/brilop.h
      include/brilprimitive.h
      include/briltype.h
      include/brilbasicblock.h
      include/brilfunction.h
      include/stringtable.h
      include/baleen_utils.h
      include/lvn.h
  )

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
set(BRILDIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/bril)
add_executable(test_runner tests/test_runner.cpp)

target_link_libraries(test_runner baleen)

macro(BALEEN_TEST name)
  add_test(NAME ${name} COMMAND test_runner ${BRILDIR}/${name}.json)
endmacro()
BALEEN_TEST(add)
BALEEN_TEST(call)
BALEEN_TEST(eight-queens)
BALEEN_TEST(ptr)
BALEEN_TEST(br)
BALEEN_TEST(dead_block)
BALEEN_TEST(dead_jmp)
BALEEN_TEST(dead_func)
BALEEN_TEST(dead_call)
BALEEN_TEST(dead_nested_call)
BALEEN_TEST(unused_vars)
BALEEN_TEST(unused_nested_vars)
BALEEN_TEST(dead_def)
BALEEN_TEST(simple_cse)
BALEEN_TEST(redef_cse)
BALEEN_TEST(assoc_cse)
BALEEN_TEST(assoc2_cse)
BALEEN_TEST(func_cse)
BALEEN_TEST(copy_propogation)
BALEEN_TEST(constant_propogation)
BALEEN_TEST(dataflow)

add_executable(stringtable_tester tests/stringtable_tester.cpp)
target_link_libraries(stringtable_tester baleen)
add_test(NAME stringtable COMMAND stringtable_tester)

add_executable(brilopt src/brilopt.cpp)
target_link_libraries(brilopt baleen)
