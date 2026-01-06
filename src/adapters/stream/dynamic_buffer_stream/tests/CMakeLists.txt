add_executable(
    unit_test_dynamic_buffer_stream
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_dynamic_buffer_stream.c
)

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

target_include_directories(
    unit_test_dynamic_buffer_stream
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/adapters/stream/dynamic_buffer_stream/tests/include"
    "${CMAKE_SOURCE_DIR}/src/foundation/osal/tests/support/include"
    "${CMAKE_SOURCE_DIR}/tests/test_support/lexleo_cmocka_support/include"
    "${CMAKE_SOURCE_DIR}/src/osal/support/include"
)

add_test(
    NAME unit_test_dynamic_buffer_stream
    COMMAND unit_test_dynamic_buffer_stream
)