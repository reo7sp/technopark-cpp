cmake_minimum_required(VERSION 3.6)
project(technopark_cpp)

set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -g")

set(SOURCE_FILES a.c)
add_executable(a a.c)
add_executable(b b.c)
