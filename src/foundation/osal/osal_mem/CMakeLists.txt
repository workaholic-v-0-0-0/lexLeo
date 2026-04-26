# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    osal_mem
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_mem.c"
)

target_include_directories(
    osal_mem
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_mem
    PUBLIC
        lexleo_policy_headers
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
