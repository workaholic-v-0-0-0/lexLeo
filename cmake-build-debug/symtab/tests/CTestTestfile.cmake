# CMake generated Testfile for 
# Source directory: /home/workaholic/env/clion/workspace/lexLeo/symtab/tests
# Build directory: /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[symtabTests]=] "/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab/tests/test_symtab")
set_tests_properties([=[symtabTests]=] PROPERTIES  LABELS "symtab_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/symtab/tests/CMakeLists.txt;14;add_test;/home/workaholic/env/clion/workspace/lexLeo/symtab/tests/CMakeLists.txt;0;")
add_test([=[symtabTestsValgrind]=] "valgrind" "--leak-check=full" "--error-exitcode=1" "--track-origins=yes" "--show-leak-kinds=all" "./test_symtab")
set_tests_properties([=[symtabTestsValgrind]=] PROPERTIES  LABELS "memory_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/symtab/tests/CMakeLists.txt;17;add_test;/home/workaholic/env/clion/workspace/lexLeo/symtab/tests/CMakeLists.txt;0;")
