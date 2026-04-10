# src/cli/CMakeLists.txt

set(
    LOG_PATH_PROVIDER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src/platform"
)

set(
    LOG_PATH_PROVIDER_SOURCE
    "$<$<PLATFORM_ID:Windows>:${LOG_PATH_PROVIDER_DIR}/cli_log_path_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${LOG_PATH_PROVIDER_DIR}/cli_log_path_macos.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${LOG_PATH_PROVIDER_DIR}/cli_log_path_linux.c>"
)

add_library(
    lexleo_cli
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cli.c"
        ${LOG_PATH_PROVIDER_SOURCE}
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
        osal_stdio
        osal_mem
)

add_executable(
    lexLeo
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
)

target_link_libraries(
    lexLeo
    PRIVATE
        lexleo_cli
)
