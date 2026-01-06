add_library(stream_test_headers INTERFACE)
target_include_directories(
    stream_test_headers
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/include"
)
