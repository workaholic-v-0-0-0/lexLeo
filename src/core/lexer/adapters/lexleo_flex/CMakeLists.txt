# src/core/lexer/adapters/lexleo_flex/CMakeLists.txt

find_package(FLEX REQUIRED)

set(
    FLEX_L
    "src/lexer.l"
)

set(
    FLEX_SRC_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set(
    FLEX_INC_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

file(MAKE_DIRECTORY "${FLEX_SRC_DIR}")
file(MAKE_DIRECTORY "${FLEX_INC_DIR}")

set(FLEX_C "${FLEX_SRC_DIR}/lexer.yy.c")
set(FLEX_H "${FLEX_INC_DIR}/lexer.yy.h")

# Generate lexer.yy.c and lexer.yy.h
FLEX_TARGET(
    lexleo_flex_scanner
    "${FLEX_L}"
    "${FLEX_C}"
    DEFINES_FILE "${FLEX_H}"
    COMPILE_FLAGS "--reentrant"
)

add_library(
    lexleo_integration_flex
    STATIC
    "src/lexleo_flex.c"
    ${FLEX_lexleo_flex_scanner_OUTPUTS}
)

target_link_libraries(
    lexleo_integration_flex
    PRIVATE
        osal_mem
        lexleo_panic
)

target_include_directories(lexleo_integration_flex
    PUBLIC
    "include"
    "${CMAKE_SOURCE_DIR}/src/langage/include"
    "${CMAKE_SOURCE_DIR}/src/policy/include"
    PRIVATE
    "src"
    "${FLEX_INC_DIR}"
    "${FLEX_SRC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/internal"
    "${CMAKE_SOURCE_DIR}/src/foundation/input_provider_legacy/include"
    "${CMAKE_SOURCE_DIR}/src/core/lexer/port/include"
    "${CMAKE_SOURCE_DIR}"
)
