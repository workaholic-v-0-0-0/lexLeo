add_library(
    stream_white_box_tests_access
    INTERFACE
)

target_include_directories(
    stream_white_box_tests_access
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
)
