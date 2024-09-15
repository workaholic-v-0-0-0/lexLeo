project(lexLeo_data_structures)

file(GLOB SRC_FILES "src/*.c")
add_library(${PROJECT_NAME} ${SRC_FILES})
add_library(lexLeo::data_structures ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC lexLeo::logger)

add_subdirectory(tests)
