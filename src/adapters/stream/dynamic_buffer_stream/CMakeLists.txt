add_library(dynamic_buffer_stream STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream.c"
)

target_link_libraries(dynamic_buffer_stream PRIVATE stream osal)

target_include_directories(dynamic_buffer_stream
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/port/stream"
    "${CMAKE_SOURCE_DIR}/src/policy/include"
)

add_subdirectory(tests)
