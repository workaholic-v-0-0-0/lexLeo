# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    dynamic_buffer_stream_new_white_box_tests_access
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream_white_box_tests_access.c"
)

target_include_directories(
    dynamic_buffer_stream_new_white_box_tests_access
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/"
)

target_link_libraries(
    dynamic_buffer_stream_new_white_box_tests_access
    PUBLIC
        dynamic_buffer_stream_new_cr
        osal_mem
)
