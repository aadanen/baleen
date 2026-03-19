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
      include/stringtable.h
  )

enable_testing()
set(BRILDIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/bril)
add_executable(test_runner tests/test_runner.cpp)

target_link_libraries(test_runner baleen)

macro(BRILTEST name)
  add_test(NAME ${name} COMMAND test_runner ${BRILDIR}/${name}.json)
endmacro()
BRILTEST(add)
BRILTEST(call)
BRILTEST(eight-queens)
BRILTEST(ptr)

add_executable(stringtable_tester tests/stringtable_tester.cpp)
target_link_libraries(stringtable_tester baleen)
add_test(NAME stringtable COMMAND stringtable_tester)
