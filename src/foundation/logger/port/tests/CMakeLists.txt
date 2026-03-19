# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/logger/port/tests/
# CMakeLists.txt

add_executable(
    unit_test_logger
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_logger.c"
)

target_link_libraries(
    unit_test_logger
    PRIVATE
    logger
    osal_time_fake_provider
    osal_mem_fake_provider
    lexleo_policy_headers
    lexleo_cmocka_support
    cmocka
)

add_test(
    NAME unit_test_logger
    COMMAND unit_test_logger
)

add_executable(
    integration_test_logger
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/integration_test_logger_default.c"
)

target_link_libraries(
    integration_test_logger
    PRIVATE
        logger
        logger_default
        osal_time_fake_provider
        osal_mem_fake_provider
        stream_fake_provider
        lexleo_cmocka_support
        cmocka
)

add_test(
    NAME integration_test_logger
    COMMAND integration_test_logger
)
