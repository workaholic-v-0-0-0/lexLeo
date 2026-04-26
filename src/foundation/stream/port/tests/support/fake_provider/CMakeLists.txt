# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream_fake_provider
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_fake_provider.c"
)

target_include_directories(
    stream_fake_provider
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stream_fake_provider
    PUBLIC
        stream
        lexleo_policy_headers
    PRIVATE
        fake_stream
        osal_str
        osal_mem_fake_provider
)
