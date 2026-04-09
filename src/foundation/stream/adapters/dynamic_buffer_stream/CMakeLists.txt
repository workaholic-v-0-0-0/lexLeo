# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    dynamic_buffer_stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream.c"
)
target_include_directories(
    dynamic_buffer_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    dynamic_buffer_stream
    PUBLIC
        stream
        osal_mem
        lexleo_policy_headers
    PRIVATE
        lexleo_assert
)

add_subdirectory(tests)
