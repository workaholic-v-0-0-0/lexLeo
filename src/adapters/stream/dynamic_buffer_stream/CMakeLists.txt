add_library(
    dynamic_buffer_stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream.c"
)
target_include_directories(
    dynamic_buffer_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    dynamic_buffer_stream
    PUBLIC
        stream
    PRIVATE
        osal
        lexleo_assert
)

add_library(dynamic_buffer_stream_wiring_headers INTERFACE)
target_link_libraries(
    dynamic_buffer_stream_wiring_headers
    INTERFACE
        dynamic_buffer_stream
        stream_wiring_headers
        osal_wiring_headers
)

add_subdirectory(tests)
