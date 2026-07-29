# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_str
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_str.c"
)

target_include_directories(
    fake_str
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_str
    PUBLIC
        osal_mem
        lexleo_policy_headers
    PRIVATE
        osal_str
        lexleo_cmocka_support
)
