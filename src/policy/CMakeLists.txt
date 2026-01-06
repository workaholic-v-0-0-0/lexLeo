# src/policy/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

add_library(lexleo_panic STATIC
    src/lexleo_panic.c
)
target_include_directories(lexleo_panic
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(lexleo_assert INTERFACE)
target_include_directories(lexleo_assert
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
