# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_osal_str
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_osal_str.c"
)

target_link_libraries(
    unit_test_osal_str
    PRIVATE
        osal_str
        osal_mem
        fake_memory
        osal_mem_fake_provider
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_osal_str
    COMMAND unit_test_osal_str
)
