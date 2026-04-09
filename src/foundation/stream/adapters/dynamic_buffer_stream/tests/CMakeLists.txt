# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_dynamic_buffer_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_dynamic_buffer_stream.c"
)

target_link_libraries(
    unit_test_dynamic_buffer_stream
    PRIVATE
        dynamic_buffer_stream
        dynamic_buffer_stream_white_box_tests_access
        stream
        stream_white_box_tests_access
        osal_mem_fake_provider
        lexleo_cmocka_support
)

set(DYNAMIC_BUFFER_STREAM_WHITE_BOX_INTERNAL_HEADERS_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

target_include_directories(
    unit_test_dynamic_buffer_stream
    PRIVATE
        ${DYNAMIC_BUFFER_STREAM_WHITE_BOX_INTERNAL_HEADERS_DIR}
)

add_test(
    NAME unit_test_dynamic_buffer_stream
    COMMAND unit_test_dynamic_buffer_stream
)

add_subdirectory(support)
