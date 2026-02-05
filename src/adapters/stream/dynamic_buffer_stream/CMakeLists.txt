# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/adapters/stream/dynamic_buffer_stream/
# CMakeLists.txt

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
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(dynamic_buffer_stream_wiring_headers INTERFACE)
target_link_libraries(
    dynamic_buffer_stream_wiring_headers
    INTERFACE
        dynamic_buffer_stream
        stream_wiring_headers
        osal_mem_wiring_headers
)

add_subdirectory(tests)
