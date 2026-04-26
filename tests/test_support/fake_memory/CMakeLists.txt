# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fake_memory
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_memory.c"
)

target_include_directories(
    fake_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    fake_memory
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
)

target_compile_features(fake_memory PUBLIC c_std_11)
