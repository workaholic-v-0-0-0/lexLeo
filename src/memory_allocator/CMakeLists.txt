# src/memory_allocator/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(memory_allocator LANGUAGES C)

add_library(memory_allocator ${CMAKE_CURRENT_SOURCE_DIR}/src/memory_allocator.c)
target_include_directories(memory_allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(memory_allocator PRIVATE $<$<CONFIG:Debug>:DEBUG>)
