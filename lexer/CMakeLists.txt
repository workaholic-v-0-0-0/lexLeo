project (lexLeo_lexer)

find_package(FLEX REQUIRED)
set(FLEX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l")
set(FLEX_OUT "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.yy.c")
FLEX_TARGET(lexer_source ${FLEX_SRC} ${FLEX_OUT})
add_library(${PROJECT_NAME} STATIC ${FLEX_lexer_source_OUTPUTS})
add_library(lexLeo::lexer ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PARSER_INCLUDE_DIR}
)

# lexer's source includes the parser's header
add_custom_target(prepare_headers DEPENDS ${BISON_ParserSource_OUTPUTS})
add_dependencies(${PROJECT_NAME} prepare_headers)

