# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/workaholic/env/clion/workspace/lexLeo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug

# Utility rule file for run_specific_tests_var.

# Include any custom commands dependencies for this target.
include tests/CMakeFiles/run_specific_tests_var.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/run_specific_tests_var.dir/progress.make

tests/CMakeFiles/run_specific_tests_var: data_structures/tests/test_list
tests/CMakeFiles/run_specific_tests_var: data_structures/tests/test_hashtable
tests/CMakeFiles/run_specific_tests_var: symtab/tests/test_symtab
tests/CMakeFiles/run_specific_tests_var: logger/liblexLeo_logger.a
	/usr/bin/ctest --verbose -L memory_tests

run_specific_tests_var: tests/CMakeFiles/run_specific_tests_var
run_specific_tests_var: tests/CMakeFiles/run_specific_tests_var.dir/build.make
.PHONY : run_specific_tests_var

# Rule to build all files generated by this target.
tests/CMakeFiles/run_specific_tests_var.dir/build: run_specific_tests_var
.PHONY : tests/CMakeFiles/run_specific_tests_var.dir/build

tests/CMakeFiles/run_specific_tests_var.dir/clean:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/tests && $(CMAKE_COMMAND) -P CMakeFiles/run_specific_tests_var.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/run_specific_tests_var.dir/clean

tests/CMakeFiles/run_specific_tests_var.dir/depend:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/workaholic/env/clion/workspace/lexLeo /home/workaholic/env/clion/workspace/lexLeo/tests /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/tests /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/tests/CMakeFiles/run_specific_tests_var.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tests/CMakeFiles/run_specific_tests_var.dir/depend

