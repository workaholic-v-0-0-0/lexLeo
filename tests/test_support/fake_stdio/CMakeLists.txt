# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_stdio
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_stdio.c"
)

target_include_directories(
    fake_stdio
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_stdio
    PUBLIC
        osal_stdio_api
        lexleo_policy_headers
    PRIVATE
        osal_mem
        lexleo_cmocka_support
)
