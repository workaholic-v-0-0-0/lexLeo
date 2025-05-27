# src/logger/tests/tests.cmake

add_executable(
    test_logger
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/fake_time.c # shadow time libc definition
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_logger.c
)
target_include_directories(
    test_logger
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" # for fake_time.h
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_logger PRIVATE logger ${CMOCKA_LIBRARY})
add_test(NAME test_logger COMMAND test_logger)
target_compile_definitions(test_logger PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# fake time zone
# note: FAKE_TIME_ZONE is set in src/osal/CMakeLists.txt
set_tests_properties(test_logger PROPERTIES ENVIRONMENT "${FAKE_TIME_ZONE}")

