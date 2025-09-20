# src/parser/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(parser LANGUAGES C)

find_package(BISON REQUIRED)

set(
    PARSER_Y
    "${CMAKE_CURRENT_BINARY_DIR}/bison/parser.y"
)

set(
    PARSER_Y_TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/bison/parser.y.in"
)

set(
    GENERATED_PARSER_SOURCE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set(
    GENERATED_PARSER_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include/"
)

set(
    PARSER_SRC
    "${GENERATED_PARSER_SOURCE_DIR}/parser.tab.c"
)

set(
    PARSER_HEADER
    "${GENERATED_PARSER_INCLUDE_DIR}/parser.tab.h"
)

file(MAKE_DIRECTORY ${GENERATED_PARSER_SOURCE_DIR})
file(MAKE_DIRECTORY ${GENERATED_PARSER_INCLUDE_DIR})

# will be changed to the full parser when archived
set(
    TERMINAL_LEXEMS_DECLARATION
    "\
%token <int_value> INTEGER\n\
%token <string_value> STRING\n\
%destructor { free($$); } STRING\n\
%token <symbol_name_value> SYMBOL_NAME\n\
%token LBRACE
%token RBRACE
%token LPARENTHESIS
%token RPARENTHESIS
%token SEMICOLON\n\
%token EQUAL\n\
%token READ
%token WRITE
%token DEFINE
%token CALL
"
)
set(NON_TERMINAL_LEXEMS_LIST "\n\
number_atom \
string_atom \
symbol_name_atom \
atom \
binding \
reading \
writing \
statement \
block_items \
translation_unit")
set(START_SYMBOL "translation_unit")
set(
    GRAMMAR_RULES_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/bison/grammar_rules"
)
file(GLOB GRAMMAR_RULE_FILES "${GRAMMAR_RULES_DIR}/*.y")
set(GRAMMAR_RULES "")
foreach (_f IN LISTS GRAMMAR_RULE_FILES)
    file(READ "${_f}" _content)
    string(APPEND GRAMMAR_RULES "${_content}\n")
endforeach ()
configure_file(
    ${PARSER_Y_TEMPLATE}
    "${PARSER_Y}"
    @ONLY
)

BISON_TARGET(parser ${PARSER_Y} ${PARSER_SRC} DEFINES_FILE ${PARSER_HEADER})
add_library(parser STATIC ${BISON_parser_OUTPUTS})
target_include_directories(
    parser
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/ast/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(parser PRIVATE lexer ast)

if (BUILD_TESTING)
    target_compile_definitions(parser PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
