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
        osal_file
        osal_str
        osal_env
        lexleo_policy_headers
)

add_executable(
    lexleo
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
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
