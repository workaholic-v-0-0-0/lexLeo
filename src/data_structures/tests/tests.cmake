# src/data_structures/CMakeLists.txt

# pure unit tests

add_executable(
    test_list
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_list.c
)
target_include_directories(
    test_list
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_list PRIVATE data_structures ${CMOCKA_LIBRARY})
target_compile_definitions(test_list PRIVATE $<$<CONFIG:Debug>:DEBUG>)

add_test(NAME test_list COMMAND test_list)

add_test(
    NAME test_list_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_list>
)
set_tests_properties(test_list_memory PROPERTIES LABELS "memory")


# quasi-unit test (integrating standard library)

add_executable(
    test_list_with_real_malloc
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_list_with_real_malloc.c
)
target_include_directories(
    test_list_with_real_malloc
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(
    test_list_with_real_malloc
    PRIVATE data_structures ${CMOCKA_LIBRARY})
target_compile_definitions(test_list PRIVATE $<$<CONFIG:Debug>:DEBUG>)

add_test(NAME test_list_with_real_malloc COMMAND test_list_with_real_malloc)

add_test(
    NAME test_list_with_real_malloc_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_list_with_real_malloc>
)
set_tests_properties(
    test_list_with_real_malloc_memory
    PROPERTIES LABELS "memory"
)
