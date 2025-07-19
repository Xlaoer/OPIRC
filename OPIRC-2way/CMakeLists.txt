cmake_minimum_required(VERSION 3.5)

project(OPIRC2way)

set(CMAKE_CXX_STANDARD 11)

add_executable(OPIRC2way OPIRC2way.cpp sserangecoder.cpp packagemerge.c)

target_compile_options(OPIRC2way PRIVATE "-O3")
