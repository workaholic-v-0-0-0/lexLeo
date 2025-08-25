# src/parser/tests/tests.cmake

add_executable(
    test_number_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_number_parser.c
)

target_include_directories(
    test_number_parser
    PRIVATE
    "${CMOCKA_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}/src/lexer/include"
    "${PARSER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/data_structures/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_dependencies(test_number_parser move_generated_lexer_header)

target_link_libraries(test_number_parser PRIVATE parser ${CMOCKA_LIBRARY} lexer data_structures)
target_compile_definitions(test_number_parser PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_number_parser COMMAND test_number_parser)
add_test(
    NAME test_number_parser_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_number_parser>
)
set_tests_properties(test_number_parser_memory PROPERTIES LABELS "memory")
