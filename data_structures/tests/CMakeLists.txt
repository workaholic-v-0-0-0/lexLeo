project(data_stuctures_test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED IMPORTED_TARGET check)

include_directories(${CHECK_INCLUDE_DIRS})
include_directories(../include)
add_executable(test_list test_list.c)
add_executable(test_hashtable test_hashtable.c)

target_link_libraries(test_list PkgConfig::CHECK lexLeo::data_structures)
target_link_libraries(test_hashtable PkgConfig::CHECK lexLeo::data_structures)

add_test(NAME ListTests COMMAND test_list)
set_tests_properties(ListTests PROPERTIES LABELS "data_stuctures_tests")
add_test(NAME ListTestsValgrind
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        --track-origins=yes
        --show-leak-kinds=all
        ./test_list)
set_tests_properties(ListTestsValgrind PROPERTIES LABELS "data_stuctures_tests")
set_tests_properties(ListTestsValgrind PROPERTIES LABELS "memory_tests")

add_test(NAME hashtableTests COMMAND test_hashtable)
set_tests_properties(hashtableTests PROPERTIES LABELS "data_stuctures_tests")
add_test(NAME hashtableTestsValgrind
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        --track-origins=yes
        --show-leak-kinds=all
        ./test_hashtable)
set_tests_properties(hashtableTestsValgrind PROPERTIES LABELS "data_stuctures_tests")
set_tests_properties(hashtableTestsValgrind PROPERTIES LABELS "memory_tests")
