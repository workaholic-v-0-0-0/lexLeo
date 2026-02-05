add_library(
    fs_stream
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fs_stream.c"
)

target_include_directories(
    fs_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    fs_stream
    PRIVATE
        stream
        osal_mem_legacy
        osal_file_legacy
        lexleo_policy_headers
        lexleo_assert
)

add_library(fs_stream_cr_headers INTERFACE)
target_include_directories(
    fs_stream_cr_headers
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(
    fs_stream_cr_headers
    INTERFACE
        fs_stream
        stream_cr_headers
        osal_mem_cr_headers
        osal_file_cr_headers
)

add_subdirectory(tests)
