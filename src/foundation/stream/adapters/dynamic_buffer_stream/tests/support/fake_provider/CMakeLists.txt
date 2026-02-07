# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/stream/adapters/dynamic_buffer_stream/tests/support/fake_provider/
# CMakeLists.txt
#
# -----------------------------------------------------------------------------
# dynamic_buffer_stream_fake_provider
#
# Lightweight test-support library that exposes helpers to retrieve and use
# fake dynamic_buffer_stream components for test wiring and injection. Reusable
# fakes are implemented and maintained under lexleo/tests/test_support/.
#
# NOTE: Test-only target. Do not link from production code.
# -----------------------------------------------------------------------------

add_library(
    dynamic_buffer_stream_fake_provider
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream_fake_provider.c"
)

target_include_directories(
    dynamic_buffer_stream_fake_provider
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    dynamic_buffer_stream_fake_provider
    PUBLIC
    fake_dynamic_buffer_stream
    dynamic_buffer_stream_wiring_headers
)
