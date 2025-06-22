# src/ast/tests/tests.cmake

add_executable(
    test_ast
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_ast.c
)
target_include_directories(
    test_ast
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/ast/include"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_ast PRIVATE ast ${CMOCKA_LIBRARY})
target_compile_definitions(test_ast PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_ast COMMAND test_ast)
add_test(
    NAME test_ast_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_ast>
)
set_tests_properties(test_ast_memory PROPERTIES LABELS "memory")

message(STATUS "HELLO")