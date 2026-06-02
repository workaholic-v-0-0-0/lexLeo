# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    lexleo_cli
    STATIC
        "src/cli.c"
)

target_include_directories(
    lexleo_cli
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    lexleo_cli
    PRIVATE
        lexleo_app
)

add_executable(
    lexleo
    "src/main.c"
)

target_link_libraries(
    lexleo
    PRIVATE
        lexleo_cli
)

install(
    TARGETS lexleo
    RUNTIME DESTINATION bin
)

set_target_properties(
    lexleo
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
