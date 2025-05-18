# src/logger/tests/tests.cmake

message(STATUS ">>> src/logger/tests/tests.cmake inclus")

add_executable(
    test_init_logger
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_init_logger.c
)
target_include_directories(
    test_init_logger
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_init_logger PRIVATE logger ${CMOCKA_LIBRARY})
add_test(NAME test_init_logger COMMAND test_init_logger)
