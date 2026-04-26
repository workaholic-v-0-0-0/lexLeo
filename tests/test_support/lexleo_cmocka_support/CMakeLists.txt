# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(lexleo_cmocka_support INTERFACE)

target_include_directories(
    lexleo_cmocka_support
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    lexleo_cmocka_support
    INTERFACE
        cmocka
        lexleo_policy_headers
)
