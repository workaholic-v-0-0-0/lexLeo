# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_time
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_time.c"
)

target_include_directories(
    fake_time
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_time
    PUBLIC
        osal_time
        lexleo_policy_headers
)
