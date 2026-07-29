# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_logger_new_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_logger_cr.c"
)
target_link_libraries(
    unit_test_logger_new_cr
    PRIVATE
        logger_new_cr
        logger_new_white_box_tests_access
        logger_fake_adapter
        osal_mem_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_logger_new_cr
    COMMAND unit_test_logger_new_cr
)

add_executable(
    unit_test_logger_new_borrower
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_logger_borrower.c"
)
target_link_libraries(
    unit_test_logger_new_borrower
    PRIVATE
        logger_new_borrower
        logger_new_cr
        logger_new_white_box_tests_access
        logger_fake_adapter
        osal_mem_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_logger_new_borrower
    COMMAND unit_test_logger_new_borrower
)
