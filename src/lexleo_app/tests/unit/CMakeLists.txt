# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_lexleo_app_cr
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_lexleo_app_cr.c"
)

target_link_options(
    unit_test_lexleo_app_cr
    PRIVATE
        "LINKER:--wrap=lexleo_vm_run"
        "LINKER:--wrap=lexleo_vm_destroy"
)

target_include_directories(
    unit_test_lexleo_app_cr
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr/"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

target_link_libraries(
    unit_test_lexleo_app_cr
    PRIVATE
        lexleo_app_white_box_tests_access_cr_wrapper
        lexleo_app_white_box_tests_access
        fake_vm
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
    NAME unit_test_lexleo_app_cr
    COMMAND unit_test_lexleo_app_cr
)
