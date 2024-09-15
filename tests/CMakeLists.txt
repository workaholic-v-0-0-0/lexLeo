project (lexLeo_tests)

add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Custom target to run tests grouped by labels.
# This target will be executed by default with `make` due to the ALL option.
set(TEST_LABELS "data_stuctures_tests|memory_tests")
#[[add_custom_target(run_specific_tests_var ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose -L memory_tests #data_stuctures_tests # symtab_tests memory_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS
        test_list
        test_hashtable
        test_symtab
        lexLeo::logger
)]]
