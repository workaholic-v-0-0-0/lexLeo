# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_lexleo_vm_cr_stream_creators
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_lexleo_vm_cr_stream_creators.c"
)

target_link_libraries(
    unit_test_lexleo_vm_cr_stream_creators
    PRIVATE
        lexleo_vm_white_box_tests_access
        osal_mem_fake_provider
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_lexleo_vm_cr_stream_creators
    COMMAND unit_test_lexleo_vm_cr_stream_creators
)
