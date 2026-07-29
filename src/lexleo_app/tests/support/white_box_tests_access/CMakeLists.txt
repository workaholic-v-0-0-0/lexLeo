# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    lexleo_app_white_box_tests_access
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lexleo_app_white_box_tests_access.c"
)

target_include_directories(
    lexleo_app_white_box_tests_access
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/"
)

target_link_libraries(
    lexleo_app_white_box_tests_access
    PUBLIC
        lexleo_app_cr_opaque
        stream_new_borrower
        logger_new_borrower
        lexleo_vm_borrower
        osal_mem
        osal_stdio
        osal_file
        osal_str
        osal_time
    PRIVATE
        lexleo_assert
)
