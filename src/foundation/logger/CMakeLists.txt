# src/foundation/logger/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(logger LANGUAGES C)

add_library(logger src/logger.c)
target_include_directories(logger PUBLIC include)
target_compile_definitions(logger PRIVATE $<$<CONFIG:Debug>:DEBUG>)

if(BUILD_TESTING)
    include(tests/tests.cmake)
endif()
