# src/policy/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

add_library(lexleo_policy STATIC
    src/lexleo_panic.c
)

add_library(lexleo::policy ALIAS lexleo_policy)

target_include_directories(lexleo_policy
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
