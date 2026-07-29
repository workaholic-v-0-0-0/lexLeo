# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    lexleo_app_borrower
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/borrowers/lexleo_app_borrower.c"
)
target_include_directories(
    lexleo_app_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set(
    LOG_PATH_PROVIDER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/platform"
)

set(
    LOG_PATH_PROVIDER_SOURCE
    "$<$<PLATFORM_ID:Windows>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_macos.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_linux.c>"
)

add_library(
    lexleo_app_cr_opaque
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/lexleo_app_cr.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/lexleo_app_log_path.c"
        ${LOG_PATH_PROVIDER_SOURCE}
)

target_include_directories(
    lexleo_app_cr_opaque
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr_opaque"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(

    lexleo_app_cr_opaque

    PUBLIC

        lexleo_app_borrower

    PRIVATE

        osal_mem
        osal_stdio
        osal_file
        osal_str
        osal_time
        osal_env

        stream_new_cr
        stdio_stream_new_cr
        fs_stream_new_cr

        logger_new_cr
        logger_new_adapter
        logger_default_cr

        lexleo_vm_cr

        lexleo_policy_headers
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()

