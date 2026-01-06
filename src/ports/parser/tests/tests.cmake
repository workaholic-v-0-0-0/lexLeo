# src/ports/parser/tests/CMakeLists.txt

# build unit parsers to do TDD

include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/unit_parsers_config.cmake")

# build unit parsers
include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/build_unit_parser.cmake")
build_unit_parser("number_atom" "number_atom")
build_unit_parser("string_atom" "string_atom")
build_unit_parser("symbol_name_atom" "symbol_name_atom")
build_unit_parser("atom" "atom")
build_unit_parser("binding" "binding")
build_unit_parser("reading" "reading")
build_unit_parser("statement" "statement")
build_unit_parser("translation_unit" "translation_unit")
build_unit_parser("block_items" "block_items")
build_unit_parser("block" "block")
build_unit_parser("parameters" "parameters")
build_unit_parser("list_of_parameters" "list_of_parameters")
build_unit_parser("function" "function")
build_unit_parser("function_definition" "function_definition")
build_unit_parser("function_call" "function_call")
build_unit_parser("evaluable" "evaluable")

# build unit tests (related to each grammar rule action)
include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/build_unit_test.cmake")
build_unit_test("number_atom")
build_unit_test("string_atom")
build_unit_test("symbol_name_atom")
build_unit_test("atom")
build_unit_test("binding")
build_unit_test("reading")
build_unit_test("statement")
build_unit_test("translation_unit")
build_unit_test("block_items")
build_unit_test("block")
build_unit_test("parameters")
build_unit_test("list_of_parameters")
build_unit_test("function")
build_unit_test("function_definition")
build_unit_test("function_call")
build_unit_test("evaluable")


# integration tests for parser_api.c

add_executable(
    test_parser_api
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/test_parser_api.c
)

target_include_directories(
    test_parser_api
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/core/ast/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/include/"
    PRIVATE "${CMAKE_BINARY_DIR}/src/ports/parser/include/"
    PRIVATE "${CMAKE_BINARY_DIR}/src/ports/lexer_legacy/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/ports/stream_legacy/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/adapters/stream_legacy/dynamic_buffer_stream_legacy/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/foundation/input_provider_legacy/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

target_link_libraries(
    test_parser_api
    PRIVATE ${CMOCKA_LIBRARY}
    PRIVATE ast
    PRIVATE parser
    PRIVATE lexer_legacy
)
add_dependencies(test_parser_api parser lexer_legacy)
target_compile_definitions(test_parser_api PRIVATE _GNU_SOURCE)
target_compile_definitions(test_parser_api PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_parser_api COMMAND test_parser_api)
add_test(
    NAME test_parser_api_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_parser_api>
)
set_tests_properties(test_parser_api_memory PROPERTIES LABELS "memory")
