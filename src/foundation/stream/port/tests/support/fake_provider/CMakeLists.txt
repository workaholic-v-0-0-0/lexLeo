add_library(
    stream_fake_provider
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_fake_provider.c"
)

target_include_directories(
    stream_fake_provider
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stream_fake_provider
    PRIVATE
        fake_stream
        osal_mem_fake_provider
    PUBLIC
        stream
        lexleo_policy_headers
)
