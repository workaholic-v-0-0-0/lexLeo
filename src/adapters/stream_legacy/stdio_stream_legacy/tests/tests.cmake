# src/adapters/stream_legacy/stdio_stream_legacy/tests/tests.cmake

add_executable(
    unit_test_stdio_stream_legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_stdio_stream_legacy.c
)

target_link_libraries(
    unit_test_stdio_stream_legacy
    PRIVATE stdio_stream_legacy ${CMOCKA_LIBRARY} fake_memory osal
)

target_include_directories(
    unit_test_stdio_stream_legacy
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
)

target_compile_definitions(unit_test_stdio_stream_legacy PRIVATE $<$<CONFIG:Debug>:DEBUG>)

if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(unit_test_stdio_stream_legacy PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(unit_test_stdio_stream_legacy PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(unit_test_stdio_stream_legacy PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

add_test(NAME unit_test_stdio_stream_legacy COMMAND unit_test_stdio_stream_legacy)
if (UNIX AND NOT APPLE)
    add_test(
        NAME unit_test_stdio_stream_legacy_memory
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:unit_test_stdio_stream_legacy>
    )
    set_tests_properties(unit_test_stdio_stream_legacy_memory PROPERTIES LABELS "memory")
endif ()
