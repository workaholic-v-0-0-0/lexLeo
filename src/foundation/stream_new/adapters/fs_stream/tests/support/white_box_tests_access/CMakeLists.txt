# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fs_stream_new_white_box_tests_access
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fs_stream_white_box_tests_access.c"
)

target_include_directories(
    fs_stream_new_white_box_tests_access
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/"
)

target_link_libraries(
    fs_stream_new_white_box_tests_access
    PUBLIC
        fs_stream_new_cr
        osal_mem
)
