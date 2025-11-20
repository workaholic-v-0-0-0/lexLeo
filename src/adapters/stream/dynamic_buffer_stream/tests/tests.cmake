# src/adapters/stream/dynamic_buffer_stream/tests/tests.cmake

add_executable(
    unit_test_dynamic_buffer_stream
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dynamic_buffer_stream.c
)

target_link_libraries(
    unit_test_dynamic_buffer_stream
    PRIVATE dynamic_buffer_stream ${CMOCKA_LIBRARY} fake_memory osal
)

target_include_directories(
    unit_test_dynamic_buffer_stream
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
    ${CMAKE_SOURCE_DIR}/src/frontend/ports/stream/include/internal
)

target_compile_definitions(unit_test_dynamic_buffer_stream PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(unit_test_dynamic_buffer_stream PRIVATE UNIT_TEST)

if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(unit_test_dynamic_buffer_stream PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(unit_test_dynamic_buffer_stream PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(unit_test_dynamic_buffer_stream PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

add_test(NAME unit_test_dynamic_buffer_stream COMMAND unit_test_dynamic_buffer_stream)
if (UNIX AND NOT APPLE)
    add_test(
        NAME unit_test_dynamic_buffer_stream_memory
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:unit_test_dynamic_buffer_stream>
    )
    set_tests_properties(unit_test_dynamic_buffer_stream_memory PROPERTIES LABELS "memory")
endif ()
