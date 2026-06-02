# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream_white_box_tests_access
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_white_box_tests_access.c"
)

target_include_directories(
    stream_white_box_tests_access
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/"
)

target_link_libraries(
    stream_white_box_tests_access
    PUBLIC
        stream_cr
    PRIVATE
        osal_mem
)
