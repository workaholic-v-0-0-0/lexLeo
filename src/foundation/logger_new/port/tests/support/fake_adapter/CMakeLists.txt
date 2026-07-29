# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_fake_adapter
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_fake_adapter.c"
)

target_include_directories(
    logger_fake_adapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_link_libraries(
    logger_fake_adapter
    PUBLIC
        logger_new_adapter
    PRIVATE
        lexleo_cmocka_support
)
