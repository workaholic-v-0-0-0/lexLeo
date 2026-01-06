add_executable(
    unit_test_dynamic_buffer_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_dynamic_buffer_stream.c"
)

add_library(dynamic_buffer_stream_test_headers INTERFACE)
target_include_directories(
    dynamic_buffer_stream_test_headers
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/include"
)

add_library(dynamic_buffer_stream_test_usage_requirements INTERFACE)
target_link_libraries(
    dynamic_buffer_stream_test_usage_requirements
    INTERFACE
        dynamic_buffer_stream_wiring_headers
        dynamic_buffer_stream_test_headers
        stream
        osal_mem_fake_provider
)
# <here> repair compile and change name/refact with _test_support naming or maybe <module>_fake_provider???
#[[
target_link_libraries(
    unit_test_dynamic_buffer_stream
    PRIVATE
    stream
    dynamic_buffer_stream
    osal
    osal_mem_test_api
    lexleo_cmocka_support
    cmocka
)
#]]
target_link_libraries(
    unit_test_dynamic_buffer_stream
    PRIVATE
    dynamic_buffer_stream_test_usage_requirements
    lexleo_cmocka_support
    cmocka
)

target_link_libraries(unit_test_dynamic_buffer_stream PRIVATE
    dynamic_buffer_stream_test_headers
)

target_include_directories(
    unit_test_dynamic_buffer_stream
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/adapters/stream/dynamic_buffer_stream/tests/include"
    "${CMAKE_SOURCE_DIR}/src/foundation/osal/tests/support/include"
    "${CMAKE_SOURCE_DIR}/tests/test_support/lexleo_cmocka_support/include"
    "${CMAKE_SOURCE_DIR}/src/osal/support/include"
)

add_test(
    NAME unit_test_dynamic_buffer_stream
    COMMAND unit_test_dynamic_buffer_stream
)

#add_subdirectory(support)
