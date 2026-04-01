add_library(
    dynamic_buffer_stream_white_box_tests_access
    INTERFACE
)

target_include_directories(
    dynamic_buffer_stream_white_box_tests_access
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
)
