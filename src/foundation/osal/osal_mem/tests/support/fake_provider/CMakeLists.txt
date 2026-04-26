# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    osal_mem_fake_provider
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_mem_fake_provider.c"
)

target_include_directories(
    osal_mem_fake_provider
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_mem_fake_provider
    PUBLIC
        fake_memory
        osal_mem
)
