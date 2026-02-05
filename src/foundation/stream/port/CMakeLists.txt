# src/foundation/stream/ports/CMakeLists.txt

add_library(
    stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_factory.c"
)

target_include_directories(
    stream
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stream
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(stream_cr_headers INTERFACE)
target_link_libraries(
    stream_cr_headers
    INTERFACE
        stream
        osal_mem_cr_headers
)

add_subdirectory(tests)
