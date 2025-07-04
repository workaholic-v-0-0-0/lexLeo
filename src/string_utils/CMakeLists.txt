# src/string_utils/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(string_utils LANGUAGES C)

add_library(string_utils ${CMAKE_CURRENT_SOURCE_DIR}/src/string_utils.c)
target_include_directories(string_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(string_utils PRIVATE $<$<CONFIG:Debug>:DEBUG>)
