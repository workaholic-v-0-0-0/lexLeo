# src/lexer/tests/CMakeLists.txt

add_executable(
    test_lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_lexer.yy.c
)
target_include_directories(
    test_lexer
    PRIVATE
    "${CMOCKA_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/include"
    "${PARSER_INCLUDE_DIR}"
    "${LEXER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/data_structures/include"
)

target_link_libraries(test_lexer PRIVATE lexer ${CMOCKA_LIBRARY} data_structures)
target_compile_definitions(test_lexer PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_lexer COMMAND test_lexer)
add_test(
    NAME test_lexer_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_lexer>
)
set_tests_properties(test_lexer_memory PROPERTIES LABELS "memory")

