# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/adapters/stream/dynamic_buffer_stream/tests/
# CMakeLists.txt

add_executable(
    unit_test_dynamic_buffer_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_dynamic_buffer_stream.c"
)

add_library(dynamic_buffer_stream_test_headers INTERFACE)
target_include_directories(
    dynamic_buffer_stream_test_headers
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(dynamic_buffer_stream_test_usage_requirements INTERFACE)
target_link_libraries(
    dynamic_buffer_stream_test_usage_requirements
    INTERFACE
        dynamic_buffer_stream_wiring_headers
        dynamic_buffer_stream_test_headers
        stream
        osal_mem_fake_provider
)

target_link_libraries(
    unit_test_dynamic_buffer_stream
    PRIVATE
    dynamic_buffer_stream_test_usage_requirements
    lexleo_cmocka_support
    cmocka
)

add_test(
    NAME unit_test_dynamic_buffer_stream
    COMMAND unit_test_dynamic_buffer_stream
)

add_subdirectory(support)
