# src/symtab/tests/tests.cmake

add_executable(
    test_symtab
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_symtab.c
)
target_include_directories(
    test_symtab
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/data_structures/include"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/ast/include"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)
target_link_libraries(test_symtab PRIVATE symtab ${CMOCKA_LIBRARY})
target_compile_definitions(test_symtab PRIVATE $<$<CONFIG:Debug>:DEBUG>)
add_test(NAME test_symtab COMMAND test_symtab)
add_test(
    NAME test_symtab_memory
    COMMAND valgrind
    --leak-check=full
    --error-exitcode=1
    $<TARGET_FILE:test_symtab>
)
set_tests_properties(test_symtab_memory PROPERTIES LABELS "memory")
