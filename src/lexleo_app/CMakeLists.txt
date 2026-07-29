

set(
    LOG_PATH_PROVIDER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src/platform"
)

set(
    LOG_PATH_PROVIDER_SOURCE
    "$<$<PLATFORM_ID:Windows>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_macos.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${LOG_PATH_PROVIDER_DIR}/lexleo_app_log_path_linux.c>"
)

add_library(
    lexleo_app
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lexleo_app.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lexleo_app_log_path.c"
        ${LOG_PATH_PROVIDER_SOURCE}
)

target_include_directories(
    lexleo_app
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    lexleo_app

    PRIVATE

        osal_mem
        osal_stdio
        osal_file
        osal_str
        osal_time
        osal_env

        stream_cr
        stdio_stream
        fs_stream

        logger_adapter
        logger_default

        lexleo_vm_cr

        lexleo_policy_headers
        lexleo_assert
)
