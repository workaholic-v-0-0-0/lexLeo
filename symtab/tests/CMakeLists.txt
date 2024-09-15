project(symtab_test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED IMPORTED_TARGET check)

include_directories(${CHECK_INCLUDE_DIRS})
include_directories(../include)
add_executable(test_symtab test_symtab.c)

target_link_libraries(test_symtab PkgConfig::CHECK lexLeo::symtab lexLeo::data_structures)

enable_testing()

add_test(NAME symtabTests COMMAND test_symtab)
set_tests_properties(symtabTests PROPERTIES LABELS "symtab_tests")

add_test(NAME symtabTestsValgrind
        COMMAND valgrind
        --leak-check=full
        --error-exitcode=1
        --track-origins=yes
        --show-leak-kinds=all
        ./test_symtab)
set_tests_properties(symtabTestsValgrind PROPERTIES LABELS "symtab_tests")
set_tests_properties(symtabTestsValgrind PROPERTIES LABELS "memory_tests")
