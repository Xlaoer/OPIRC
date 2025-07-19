cmake_minimum_required(VERSION 3.5)

project(OPIRC3way)

set(CMAKE_CXX_STANDARD 11)

add_executable(OPIRC3way OPIRC3way.cpp sserangecoder.cpp packagemerge.c)

target_compile_options(OPIRC3way PRIVATE "-O3")
