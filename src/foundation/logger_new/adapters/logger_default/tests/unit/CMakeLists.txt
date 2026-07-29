# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_logger_default_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_logger_default_cr.c"
)

target_link_libraries(
    unit_test_logger_default_cr
    PRIVATE
        logger_new_adapter
        logger_new_cr
        logger_default_cr
        logger_default_white_box_tests_access
        fake_stream_new
        osal_time_fake_provider
        osal_mem_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_logger_default_cr
    COMMAND unit_test_logger_default_cr
)
