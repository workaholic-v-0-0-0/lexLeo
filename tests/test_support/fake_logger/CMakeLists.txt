# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_logger
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_logger.c"
)

target_include_directories(
    fake_logger
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_logger
    PUBLIC
        logger_new_cr
        logger_new_borrower
        fake_logger_adapter
    PRIVATE
        osal_mem
        logger_new_white_box_tests_access
        lexleo_cmocka_support
)
