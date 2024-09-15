project(lexLeo_interpreter)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME}
        lexLeo::parser
)
