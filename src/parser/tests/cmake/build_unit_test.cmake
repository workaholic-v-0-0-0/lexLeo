# src/parser/tests/cmake/build_unit_test.cmake

function(build_unit_test NAME)
    add_executable(
        "test_${NAME}_parser"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_${NAME}_parser.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/mock_lexer/src/mock_lexer.c"
    )
    string(TOUPPER "${NAME}" NAME_UPPER)
    target_compile_definitions(
        "test_${NAME}_parser"
        PRIVATE
        STYPE=${NAME_UPPER}_STYPE
        UNIT_PARSER_HEADER=\"${NAME}_parser.tab.h\"
    )
    target_include_directories(
        "test_${NAME}_parser"
        PRIVATE
        "${CMOCKA_INCLUDE_DIR}"
        "${CMAKE_BINARY_DIR}/src/lexer/include"
        "${GENERATED_PARSER_UNIT_INCLUDE_DIR}"
        "${CMAKE_SOURCE_DIR}/src/data_structures/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/mock_lexer/include"
    )
    add_dependencies("test_${NAME}_parser" move_generated_lexer_header)
    target_link_libraries("test_${NAME}_parser" PRIVATE "${NAME}_parser" ${CMOCKA_LIBRARY} lexer data_structures)
    target_compile_definitions("test_${NAME}_parser" PRIVATE $<$<CONFIG:Debug>:DEBUG>)
    add_test(NAME "test_${NAME}_parser" COMMAND "test_${NAME}_parser")
    set(TEST_TARGET "test_${NAME}_parser")
    add_test(
        NAME "${TEST_TARGET}_memory"
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:${TEST_TARGET}>
    )
    set_tests_properties("test_${NAME}_parser_memory" PROPERTIES LABELS " memory")
endfunction()
