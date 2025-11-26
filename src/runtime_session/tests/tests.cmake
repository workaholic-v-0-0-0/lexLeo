# src/runtime_session/tests/tests.cmake

add_executable(
    unit_test_runtime_session
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runtime_session.c
)

target_link_libraries(
    unit_test_runtime_session
    PRIVATE runtime_session ${CMOCKA_LIBRARY} fake_memory symtab ast
)

target_include_directories(
    unit_test_runtime_session
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
    PRIVATE "${CMAKE_SOURCE_DIR}/src/lexer/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/symtab/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/ast/include/"
)

target_compile_definitions(unit_test_runtime_session PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(unit_test_runtime_session PRIVATE UNIT_TEST)

if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(unit_test_runtime_session PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(unit_test_runtime_session PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(unit_test_runtime_session PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

if (USE_STRING_UTILS)
    target_compile_definitions(unit_test_runtime_session PRIVATE USE_STRING_UTILS)
    target_include_directories(unit_test_runtime_session PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(unit_test_runtime_session PRIVATE ${STRING_UTILS_TARGET})
endif ()

add_test(NAME unit_test_runtime_session COMMAND unit_test_runtime_session)
if (UNIX AND NOT APPLE)
    add_test(
        NAME unit_test_runtime_session_memory
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:unit_test_runtime_session>
    )
    set_tests_properties(unit_test_runtime_session_memory PROPERTIES LABELS "memory")
endif ()
