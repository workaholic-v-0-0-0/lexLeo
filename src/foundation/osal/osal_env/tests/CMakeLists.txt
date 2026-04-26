# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_osal_env
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_osal_env.c"
)

target_link_libraries(
    unit_test_osal_env
    PRIVATE
        osal_env
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_osal_env
    COMMAND unit_test_osal_env
)
