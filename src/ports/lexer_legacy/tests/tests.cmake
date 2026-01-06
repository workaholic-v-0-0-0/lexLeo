# src/ports/lexer_legacy/tests/CMakeLists.txt

add_executable(
    test_lexer_legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_lexer_legacy.yy.c
)
target_include_directories(
    test_lexer_legacy
    PRIVATE
    "${CMOCKA_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/langage/include"
    "${CMAKE_SOURCE_DIR}/src/policy/include"
    "${PARSER_INCLUDE_DIR}"
    "${LEXER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/foundation/data_structures/include"
)

target_link_libraries(test_lexer_legacy PRIVATE lexer ${CMOCKA_LIBRARY} data_structures)
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

