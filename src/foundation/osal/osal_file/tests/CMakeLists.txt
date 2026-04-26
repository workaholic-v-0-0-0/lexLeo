# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_osal_file
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_osal_file.c"
)

target_link_libraries(
    unit_test_osal_file
    PRIVATE
        osal_file
        osal_mem
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_osal_file
    COMMAND unit_test_osal_file
)
