# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_subdirectory(support)

add_executable(
    unit_test_osal_mem
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_osal_mem.c"
)

target_link_libraries(
    unit_test_osal_mem
    PRIVATE
        osal_mem
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_osal_mem
    COMMAND unit_test_osal_mem
)
