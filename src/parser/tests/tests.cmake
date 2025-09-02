# src/parser/tests/tests.cmake

# build unit parsers to do TDD

include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/unit_parsers_config.cmake")

# build unit parsers
include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/build_unit_parser.cmake")
build_unit_parser("number_atom" "number_atom")
build_unit_parser("string_atom" "string_atom")
build_unit_parser("symbol_name_atom" "symbol_name_atom")
build_unit_parser("atom" "atom")

# build unit tests (related to each grammar rule action)
include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/build_unit_test.cmake")
build_unit_test("number_atom")
build_unit_test("string_atom")
build_unit_test("symbol_name_atom")
build_unit_test("atom")
