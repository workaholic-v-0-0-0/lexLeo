# src/parser/tests/tests.cmake

# unit parsers to do TDD

set(
    GRAMMAR_RULES_UNDER_TEST_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/bison/grammar_rules"
)

set(
    GRAMMAR_RULES_STUBS_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/bison/grammar_rules_stubs"
)

set(
    PARSER_UNIT_Y_TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/bison/parser_unit.y.in"
)

set(
    GENERATED_PARSER_UNIT_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/tests"
)

set(
    GENERATED_PARSER_UNIT_SOURCE_DIR
    "${GENERATED_PARSER_UNIT_DIR}/src"
)

set(
    GENERATED_PARSER_UNIT_INCLUDE_DIR
    "${GENERATED_PARSER_UNIT_DIR}/include"
)

set(
    GENERATED_BISON_FILES_FOR_TESTS
    "${CMAKE_CURRENT_BINARY_DIR}/tests/bison"
)

file(MAKE_DIRECTORY ${GENERATED_PARSER_UNIT_DIR})
file(MAKE_DIRECTORY ${GENERATED_PARSER_UNIT_SOURCE_DIR})
file(MAKE_DIRECTORY ${GENERATED_PARSER_UNIT_INCLUDE_DIR})
file(MAKE_DIRECTORY ${GENERATED_BISON_FILES_FOR_TESTS})

# number_atom parser
set(API_PREFIX "number_atom_")
set(
    NUMBER_PARSER_Y
    "${GENERATED_BISON_FILES_FOR_TESTS}/number_parser.y"
)
set(
    NUMBER_PARSER_SRC
    "${GENERATED_PARSER_UNIT_SOURCE_DIR}/number_parser.tab.c"
)
set(
    NUMBER_PARSER_HEADER
    "${GENERATED_PARSER_UNIT_INCLUDE_DIR}/number_parser.tab.h"
)
set(TERMINAL_LEXEMS_DECLARATION "%token <int_value> INTEGER")
set(NON_TERMINAL_LEXEMS_LIST "number_atom")
set(START_SYMBOL "number_atom")
file(
    READ
    "${GRAMMAR_RULES_UNDER_TEST_DIR}/number_atom_rule.y"
    GRAMMAR_RULES_UNDER_TEST
)
#[[
file(
    READ
    "${GRAMMAR_RULES_STUBS_DIR}/number_atom_rule_stubbed_dependencies.y"
    GRAMMAR_RULES_STUB
)
]]

configure_file(
    ${PARSER_UNIT_Y_TEMPLATE}
    "${GENERATED_BISON_FILES_FOR_TESTS}/number_parser.y"
    @ONLY
)
BISON_TARGET(number_parser ${NUMBER_PARSER_Y} ${NUMBER_PARSER_SRC} DEFINES_FILE ${NUMBER_PARSER_HEADER})
add_library(number_parser STATIC ${BISON_number_parser_OUTPUTS})
target_include_directories(
    number_parser
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/include"
    "${GENERATED_PARSER_UNIT_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/ast/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(number_parser PRIVATE lexer ast)

# unit tests

add_executable(
    test_number_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_number_parser.c
)

target_include_directories(
    test_number_parser
    PRIVATE
    "${CMOCKA_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}/src/lexer/include"
    "${GENERATED_PARSER_UNIT_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/data_structures/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_dependencies(test_number_parser move_generated_lexer_header)

target_link_libraries(test_number_parser PRIVATE number_parser ${CMOCKA_LIBRARY} lexer data_structures)
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
