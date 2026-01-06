# src/core/runtime_env/tests/tests.cmake

add_executable(
    unit_test_runtime_env
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runtime_env.c
)

target_include_directories(
    unit_test_runtime_env
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    #PRIVATE "${CMAKE_SOURCE_DIR}/src/core/ast/include"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

target_link_libraries(
    unit_test_runtime_env
    PRIVATE runtime_env ${CMOCKA_LIBRARY} fake_memory #ast
)
target_compile_definitions(unit_test_runtime_env PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME unit_test_runtime_env COMMAND unit_test_runtime_env)
add_test(
    NAME unit_test_runtime_env_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:unit_test_runtime_env>
)
set_tests_properties(unit_test_runtime_env_memory PROPERTIES LABELS "memory")
