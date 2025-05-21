# src/logger/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(logger LANGUAGES C)

add_library(logger ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.c)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
