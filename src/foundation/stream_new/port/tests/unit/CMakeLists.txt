# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_stream_new_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream_cr.c"
)
target_link_libraries(
    unit_test_stream_new_cr
    PRIVATE
        stream_new_cr
        stream_new_white_box_tests_access
        stream_fake_adapter
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_stream_new_cr
    COMMAND unit_test_stream_new_cr
)

add_executable(
    unit_test_stream_new_regular_file_creator_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream_regular_file_creator_cr.c"
)
target_link_libraries(
    unit_test_stream_new_regular_file_creator_cr
    PRIVATE
        stream_new_cr
        stream_new_white_box_tests_access
        stream_fake_adapter
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_stream_new_regular_file_creator_cr
    COMMAND unit_test_stream_new_regular_file_creator_cr
)

add_executable(
    unit_test_stream_internal_api
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream_internal_api.c"
)
target_link_libraries(
    unit_test_stream_internal_api
    PRIVATE
        stream_new_cr
        stream_new_white_box_tests_access
        stream_fake_adapter
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_stream_internal_api
    COMMAND unit_test_stream_internal_api
)

add_executable(
    unit_test_stream_regular_file_creator_owner
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream_regular_file_creator_owner.c"
)
target_link_libraries(
    unit_test_stream_regular_file_creator_owner
    PRIVATE
        stream_new_cr
        stream_new_white_box_tests_access
        stream_fake_adapter
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_stream_regular_file_creator_owner
    COMMAND unit_test_stream_regular_file_creator_owner
)

add_executable(
    unit_test_stream_new_borrower
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_stream_borrower.c"
)
target_link_libraries(
    unit_test_stream_new_borrower
    PRIVATE
        stream_new_cr
        stream_new_white_box_tests_access
        stream_fake_adapter
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME unit_test_stream_new_borrower
    COMMAND unit_test_stream_new_borrower
)