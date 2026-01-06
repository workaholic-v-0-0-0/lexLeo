# src/core/resolver/tests/tests.cmake

add_executable(
    unit_test_resolver
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/test_resolver.c
)

target_include_directories(
    unit_test_resolver
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/core/ast/include"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/core/symtab/include"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

target_link_libraries(
    unit_test_resolver
    PRIVATE resolver ${CMOCKA_LIBRARY} fake_memory
)
target_compile_definitions(unit_test_resolver PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME unit_test_resolver COMMAND unit_test_resolver)
add_test(
    NAME unit_test_resolver_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:unit_test_resolver>
)
set_tests_properties(unit_test_resolver_memory PROPERTIES LABELS "memory")
target_compile_definitions(
    unit_test_resolver
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    USE_MEMORY_ALLOCATOR
)


add_executable(
    unit_test_resolver_external_doubling
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/test_resolver_external_doubling.c
)

target_include_directories(
    unit_test_resolver_external_doubling
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/core/ast/include"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/core/symtab/include"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

target_link_libraries(
    unit_test_resolver_external_doubling
    PRIVATE resolver ${CMOCKA_LIBRARY} fake_memory
)
target_compile_definitions(unit_test_resolver_external_doubling PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME unit_test_resolver_external_doubling COMMAND unit_test_resolver_external_doubling)
add_test(
    NAME unit_test_resolver_external_doubling_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:unit_test_resolver_external_doubling>
)
set_tests_properties(unit_test_resolver_external_doubling_memory PROPERTIES LABELS "memory")
target_compile_definitions(
    unit_test_resolver_external_doubling
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    USE_MEMORY_ALLOCATOR
)
