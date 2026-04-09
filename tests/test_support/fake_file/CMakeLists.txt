# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_file
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_file.c"
)

target_include_directories(
    fake_file
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_file
    PRIVATE
        osal_file
        osal_mem
        lexleo_assert
)
