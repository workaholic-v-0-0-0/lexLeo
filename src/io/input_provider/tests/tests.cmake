# src/io/input_provider/tests/tests.cmake

add_executable(
    unit_test_input_provider
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_input_provider.c
)

target_link_libraries(
    unit_test_input_provider
    PRIVATE input_provider ${CMOCKA_LIBRARY} fake_memory
)

target_include_directories(
    unit_test_input_provider
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
)

target_compile_definitions(unit_test_input_provider PRIVATE $<$<CONFIG:Debug>:DEBUG>)

if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(unit_test_input_provider PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(unit_test_input_provider PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(unit_test_input_provider PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

add_test(NAME unit_test_input_provider COMMAND unit_test_input_provider)
if (UNIX AND NOT APPLE)
    add_test(
        NAME unit_test_input_provider_memory
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:unit_test_input_provider>
    )
    set_tests_properties(unit_test_input_provider_memory PROPERTIES LABELS "memory")
endif ()
