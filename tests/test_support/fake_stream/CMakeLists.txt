# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_stream.c"
)

target_include_directories(
    fake_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_stream
    PRIVATE
        stream
        osal_mem
        osal_str
        lexleo_assert
)
