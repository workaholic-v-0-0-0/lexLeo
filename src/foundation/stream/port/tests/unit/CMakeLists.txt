# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream.c"
)

target_link_libraries(
    unit_test_stream
    PRIVATE
        stream_cr
        stream_white_box_tests_access
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_stream
    COMMAND unit_test_stream
)
