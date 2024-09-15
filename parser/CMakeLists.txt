project (lexLeo_parser)

find_package(BISON REQUIRED)
set(YACC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y")
set(YACC_OUT "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.tab.c")
set(PARSER_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/parser/parser.tab.h")
BISON_TARGET(parser_source ${YACC_SRC} ${YACC_OUT}
        DEFINES_FILE ${PARSER_HEADER})

add_library(lexLeo_parser STATIC ${BISON_parser_source_OUTPUTS})
add_library(lexLeo::parser ALIAS ${PROJECT_NAME})

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/parser"
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        lexLeo::lexer
        lexLeo::data_structures
        lexLeo::symtab
        lexLeo::ast
)

add_dependencies(
        ${PROJECT_NAME}
        lexLeo::lexer
        lexLeo::data_structures
        lexLeo::symtab
        lexLeo::ast
)
