# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_stream_adapter
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_stream_adapter.c"
)

target_include_directories(
    fake_stream_adapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_stream_adapter
    PUBLIC
        stream_new_adapter
    PRIVATE
        osal_mem
        lexleo_cmocka_support
)
