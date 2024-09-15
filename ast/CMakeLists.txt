project(lexLeo_ast)

add_library(${PROJECT_NAME} src/ast.c)
add_library(lexLeo::ast ALIAS ${PROJECT_NAME})

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        lexLeo::data_structures
        lexLeo::logger
)

add_subdirectory(tests)
