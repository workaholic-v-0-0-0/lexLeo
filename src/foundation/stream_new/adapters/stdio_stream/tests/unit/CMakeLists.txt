# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_stdio_stream_new_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stdio_stream_cr.c"
)

target_link_libraries(
    unit_test_stdio_stream_new_cr
    PRIVATE
        stream_new_adapter
        stream_new_cr
        stdio_stream_new_cr
        stdio_stream_new_white_box_tests_access
        osal_mem_fake_provider
        osal_stdio
        osal_stdio_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_stdio_stream_new_cr
    COMMAND unit_test_stdio_stream_new_cr
)
