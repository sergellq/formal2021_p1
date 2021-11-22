cmake_minimum_required(VERSION 3.20)
project(formal2021)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(formal2021 main.cpp Automaton.cpp Automaton.h Edge.h solve.h)

include_directories("." ${GTEST_INCLUDE_DIRS})

set(FILES_TEST tests/manual_tests.cpp)
set(FILES_SOURCE Automaton.cpp tests/MainTest.cpp)

add_executable(Tests tests/MainTest.cpp ${FILES_TEST} ${FILES_SOURCE})

target_link_libraries(Tests ${GTEST_LIBRARIES} Threads::Threads)