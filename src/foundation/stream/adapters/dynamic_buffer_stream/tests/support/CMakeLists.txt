# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/stream/adapters/dynamic_buffer_stream/tests/support/
# CMakeLists.txt

add_subdirectory(fake_provider)

add_library(dynamic_buffer_stream_test_support INTERFACE)
target_link_libraries(dynamic_buffer_stream_test_support INTERFACE
    dynamic_buffer_stream_fake_provider
)
