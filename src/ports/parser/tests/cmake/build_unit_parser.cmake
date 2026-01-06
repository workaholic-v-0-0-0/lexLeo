# src/ports/parser/tests/cmake/build_unit_parser.cmake

function(build_unit_parser NAME)
    list(GET ARGN 0 START_SYMBOL)
    set(
        NUMBER_ATOM_PARSER_Y
        "${GENERATED_BISON_FILES_FOR_TESTS}/${NAME}_parser.y"
    )
    set(
        NUMBER_ATOM_PARSER_SRC
        "${GENERATED_PARSER_UNIT_SOURCE_DIR}/${NAME}_parser.tab.c"
    )
    set(
        NUMBER_ATOM_PARSER_HEADER
        "${GENERATED_PARSER_UNIT_INCLUDE_DIR}/${NAME}_parser.tab.h"
    )
    file(
        READ
        "${GRAMMAR_SYMBOLS_DECLARATION_DIR}/${NAME}_grammar_symbols_declaration.y"
        GRAMMAR_SYMBOLS_DECLARATION
    )
    file(
        READ
        "${GRAMMAR_RULES_UNDER_TEST_DIR}/${NAME}_rule.y"
        GRAMMAR_RULES_UNDER_TEST
    )
    if (EXISTS "${GRAMMAR_RULES_STUBS_DIR}/${NAME}_rule_dependencies_stubs_declaration.y")
        file(READ
            "${GRAMMAR_RULES_STUBS_DIR}/${NAME}_rule_dependencies_stubs_declaration.y"
            STUBS_DECLARATION
        )
    else ()
        message(STATUS "${NAME}_rule_dependencies_stubs_declaration.y has not been found.")
    endif ()
    if (EXISTS "${GRAMMAR_RULES_STUBS_DIR}/${NAME}_rule_dependencies.y")
        file(READ
            "${GRAMMAR_RULES_STUBS_DIR}/${NAME}_rule_dependencies.y"
            GRAMMAR_RULES_STUB
        )
    endif ()
    configure_file(
        ${PARSER_UNIT_Y_TEMPLATE}
        "${GENERATED_BISON_FILES_FOR_TESTS}/${NAME}_parser.y"
        @ONLY
    )
    BISON_TARGET(
        "${NAME}_parser"
        ${NUMBER_ATOM_PARSER_Y}
        ${NUMBER_ATOM_PARSER_SRC}
        DEFINES_FILE
        ${NUMBER_ATOM_PARSER_HEADER}
    )
    set(BISON_NAME "${NAME}_parser")
    set(_bison_outputs_var "BISON_${BISON_NAME}_OUTPUTS")
    add_library(${BISON_NAME} STATIC ${${_bison_outputs_var}})
    target_include_directories(
        "${NAME}_parser"
        PUBLIC
        "${CMAKE_SOURCE_DIR}/src/policy/include"
        "${GENERATED_PARSER_UNIT_INCLUDE_DIR}"
        "${CMAKE_SOURCE_DIR}/src/core/ast/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    target_link_libraries("${NAME}_parser" PRIVATE lexer_legacy ast)
endfunction()
