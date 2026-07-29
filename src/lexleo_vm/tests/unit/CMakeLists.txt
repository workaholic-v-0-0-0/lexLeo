# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_lexleo_vm_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_lexleo_vm_cr.c"
)

target_include_directories(
    unit_test_lexleo_vm_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

target_link_libraries(
    unit_test_lexleo_vm_cr
    PRIVATE
        lexleo_vm_white_box_tests_access
        lexleo_vm_cr
        fake_stream_adapter
        fake_stream_new
        fake_logger_adapter
        fake_logger
        osal_mem_fake_provider
        osal_stdio_fake_provider
        osal_file_fake_provider
        osal_str_fake_provider
        osal_time_fake_provider
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_lexleo_vm_cr
    COMMAND unit_test_lexleo_vm_cr
)
