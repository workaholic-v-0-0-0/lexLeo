# CMake generated Testfile for 
# Source directory: /home/workaholic/env/clion/workspace/lexLeo/data_structures/tests
# Build directory: /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/data_structures/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[ListTests]=] "/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/data_structures/tests/test_list")
set_tests_properties([=[ListTests]=] PROPERTIES  LABELS "data_stuctures_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;14;add_test;/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;0;")
add_test([=[ListTestsValgrind]=] "valgrind" "--leak-check=full" "--error-exitcode=1" "--track-origins=yes" "--show-leak-kinds=all" "./test_list")
set_tests_properties([=[ListTestsValgrind]=] PROPERTIES  LABELS "memory_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;16;add_test;/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;0;")
add_test([=[hashtableTests]=] "/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/data_structures/tests/test_hashtable")
set_tests_properties([=[hashtableTests]=] PROPERTIES  LABELS "data_stuctures_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;26;add_test;/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;0;")
add_test([=[hashtableTestsValgrind]=] "valgrind" "--leak-check=full" "--error-exitcode=1" "--track-origins=yes" "--show-leak-kinds=all" "./test_hashtable")
set_tests_properties([=[hashtableTestsValgrind]=] PROPERTIES  LABELS "memory_tests" _BACKTRACE_TRIPLES "/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;28;add_test;/home/workaholic/env/clion/workspace/lexLeo/data_structures/tests/CMakeLists.txt;0;")
