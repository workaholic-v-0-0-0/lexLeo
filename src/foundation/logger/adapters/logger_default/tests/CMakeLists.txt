# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/logger/adapters/logger_default/tests/
# CMakeLists.txt

add_executable(
    unit_test_logger_default
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_tests_logger_default.c"
)

target_link_libraries(
    unit_test_logger_default
    PRIVATE
        logger_default
        logger
        stream
        stream_fake_provider
        osal_time_fake_provider
        osal_mem_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
        cmocka
)

add_test(
    NAME unit_test_logger_default
    COMMAND unit_test_logger_default
)
