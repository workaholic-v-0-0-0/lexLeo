# src/data_structures/tests/tests.cmake

# pure unit tests

add_executable(
    test_list
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_list.c
)
target_include_directories(
    test_list
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/data_structures/include/"
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

add_executable(
    test_hashtable
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hashtable.c
)
target_include_directories(
    test_hashtable
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_hashtable PRIVATE data_structures ${CMOCKA_LIBRARY})
target_compile_definitions(test_hashtable PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_hashtable COMMAND test_hashtable)

add_test(
    NAME test_hashtable_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_hashtable>
)
set_tests_properties(test_hashtable_memory PROPERTIES LABELS "memory")


# quasi-unit test (integrating standard library)

add_executable(
    test_list_with_stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_list_with_stdlib.c
)
target_include_directories(
    test_list_with_stdlib
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(
    test_list_with_stdlib
    PRIVATE data_structures ${CMOCKA_LIBRARY})
target_compile_definitions(test_list PRIVATE $<$<CONFIG:Debug>:DEBUG>)

add_test(NAME test_list_with_stdlib COMMAND test_list_with_stdlib)

add_test(
    NAME test_list_with_stdlib_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_list_with_stdlib>
)
set_tests_properties(
    test_list_with_stdlib_memory
    PROPERTIES LABELS "memory"
)
