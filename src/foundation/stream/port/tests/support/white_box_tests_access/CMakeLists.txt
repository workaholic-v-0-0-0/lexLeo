# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream_white_box_tests_access
    INTERFACE
)

target_include_directories(
    stream_white_box_tests_access
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
)
