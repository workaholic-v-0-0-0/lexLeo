# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_white_box_tests_access
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_white_box_tests_access.c"
)

target_include_directories(
    logger_white_box_tests_access
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/"
)

target_link_libraries(
    logger_white_box_tests_access
    PUBLIC
        logger_adapter
    PRIVATE
        osal_mem
)
