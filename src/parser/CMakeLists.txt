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

set(
    TERMINAL_LEXEMS_DECLARATION
    "\
%token <int_value> INTEGER\n\
%token <string_value> STRING\n\
%token <symbol_name_value> SYMBOL_NAME\n\
%token LBRACE\n\
%token RBRACE\n\
%token LPAREN\n\
%token RPAREN\n\
%token ADD\n\
%token SUBTRACT\n\
%token MULTIPLY\n\
%token DIVIDE\n\
%token SEMICOLON\n\
%token COMMA\n\
%token EQUAL\n\
%token READ\n\
%token WRITE\n\
%token DEFINE\n\
%token CALL\n\
%token QUOTE\n\
%token EVAL\n\
%token END 0 \"end of file\"\n\
%token INVALID \
%token SYMBOL \
%token SET \
%token IF \
%token THEN \
%token ELSE
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
block \
parameters \
list_of_parameters \
function \
function_definition \
arguments \
list_of_arguments \
argument_list \
function_call \
computable \
evaluable \
translation_unit \
readable\n\
eval\n\
symbol\n\
set\n\
conditional_block\n\
")
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

BISON_TARGET(
    parser
    ${PARSER_Y}
    ${PARSER_SRC}
    DEFINES_FILE
    ${PARSER_HEADER}
)
add_library(
    parser
    STATIC
    ${BISON_parser_OUTPUTS}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser_api.c"
)
target_include_directories(
    parser
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/ast/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/osal/include"
)
target_link_libraries(parser PRIVATE lexer ast osal)

if (BUILD_TESTING)
    target_compile_definitions(parser PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
