# src/core/lexer/port/CMakeLists.txt

add_library(lexer STATIC
    "src/lexer.c"
)

target_include_directories(
    lexer
    PUBLIC
        "include"
    PRIVATE
        "src"
)

target_link_libraries(
    lexer
    PRIVATE
        osal_mem_legacy
)
