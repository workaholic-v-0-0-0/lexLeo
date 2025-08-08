# src/parser/tests/tests.cmake

set(START_SYMBOL "atom")
file(
    READ
    "${CMAKE_CURRENT_SOURCE_DIR}/bison/atom_grammar.y"
    GRAMMAR_RULES
)
configure_file(
    ${PARSER_Y_TEMPLATE}
    "${CMAKE_CURRENT_BINARY_DIR}/tests/atom_parser.y"
    @ONLY
)

BISON_TARGET(
    atom_parser
    "${CMAKE_CURRENT_BINARY_DIR}/tests/atom_parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/tests/atom_parser.c"
)
add_library(
    atom_parser
    STATIC
    "${BISON_atom_parser_OUTPUTS}"
)
target_include_directories(
    atom_parser
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/ast/include"
    "${CMAKE_SOURCE_DIR}/src/symtab/include"
)
target_link_libraries(atom_parser PRIVATE lexer ast symtab)
target_compile_definitions(atom_parser PRIVATE UNIT_TEST)


# tests
add_executable(
    test_atom_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_atom_parser.c
)

target_include_directories(
    test_atom_parser
    PRIVATE
    "${CMOCKA_INCLUDE_DIR}"
    "${PARSER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/data_structures/include"
)

add_dependencies(test_atom_parser move_generated_lexer_header)

target_link_libraries(test_atom_parser PRIVATE parser ${CMOCKA_LIBRARY} lexer data_structures)
target_compile_definitions(test_atom_parser PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_atom_parser COMMAND test_atom_parser)
add_test(
    NAME test_atom_parser_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_atom_parser>
)
set_tests_properties(test_atom_parser_memory PROPERTIES LABELS "memory")
