# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

cmake_minimum_required(VERSION 3.16)

add_library(lexleo_policy_headers INTERFACE)
target_include_directories(
    lexleo_policy_headers
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(lexleo_assert INTERFACE)
target_link_libraries(
    lexleo_assert
    INTERFACE
        lexleo_policy_headers
)

add_library(
    lexleo_panic
    STATIC
        src/lexleo_panic.c
)
target_link_libraries(
    lexleo_panic
    PRIVATE
        osal_stdio
        osal_mem
        osal_str
        lexleo_policy_headers
)
