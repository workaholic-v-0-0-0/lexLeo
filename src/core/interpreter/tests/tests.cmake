# src/core/interpreter/tests/tests.cmake

add_executable(
    unit_test_interpreter
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_interpreter.c
)

target_include_directories(
    unit_test_interpreter
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/runtime_env/include
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

target_link_libraries(
    unit_test_interpreter
    PRIVATE interpreter ${CMOCKA_LIBRARY} fake_memory runtime_env #ast
)
target_compile_definitions(unit_test_interpreter PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME unit_test_interpreter COMMAND unit_test_interpreter)
add_test(
    NAME unit_test_interpreter_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:unit_test_interpreter>
)
set_tests_properties(unit_test_interpreter_memory PROPERTIES LABELS "memory")
