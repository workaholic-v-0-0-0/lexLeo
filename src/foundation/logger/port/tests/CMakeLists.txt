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
    osal_mem_fake_provider
    lexleo_policy_headers
    lexleo_cmocka_support
    cmocka
)

add_test(
    NAME unit_test_logger
    COMMAND unit_test_logger
)
