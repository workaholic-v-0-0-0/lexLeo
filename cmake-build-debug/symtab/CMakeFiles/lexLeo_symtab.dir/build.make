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

# Include any dependencies generated for this target.
include symtab/CMakeFiles/lexLeo_symtab.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include symtab/CMakeFiles/lexLeo_symtab.dir/compiler_depend.make

# Include the progress variables for this target.
include symtab/CMakeFiles/lexLeo_symtab.dir/progress.make

# Include the compile flags for this target's objects.
include symtab/CMakeFiles/lexLeo_symtab.dir/flags.make

symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o: symtab/CMakeFiles/lexLeo_symtab.dir/flags.make
symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o: /home/workaholic/env/clion/workspace/lexLeo/symtab/src/symtab.c
symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o: symtab/CMakeFiles/lexLeo_symtab.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o -MF CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o.d -o CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o -c /home/workaholic/env/clion/workspace/lexLeo/symtab/src/symtab.c

symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/lexLeo_symtab.dir/src/symtab.c.i"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/workaholic/env/clion/workspace/lexLeo/symtab/src/symtab.c > CMakeFiles/lexLeo_symtab.dir/src/symtab.c.i

symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/lexLeo_symtab.dir/src/symtab.c.s"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/workaholic/env/clion/workspace/lexLeo/symtab/src/symtab.c -o CMakeFiles/lexLeo_symtab.dir/src/symtab.c.s

# Object files for target lexLeo_symtab
lexLeo_symtab_OBJECTS = \
"CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o"

# External object files for target lexLeo_symtab
lexLeo_symtab_EXTERNAL_OBJECTS =

symtab/liblexLeo_symtab.a: symtab/CMakeFiles/lexLeo_symtab.dir/src/symtab.c.o
symtab/liblexLeo_symtab.a: symtab/CMakeFiles/lexLeo_symtab.dir/build.make
symtab/liblexLeo_symtab.a: symtab/CMakeFiles/lexLeo_symtab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library liblexLeo_symtab.a"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && $(CMAKE_COMMAND) -P CMakeFiles/lexLeo_symtab.dir/cmake_clean_target.cmake
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lexLeo_symtab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
symtab/CMakeFiles/lexLeo_symtab.dir/build: symtab/liblexLeo_symtab.a
.PHONY : symtab/CMakeFiles/lexLeo_symtab.dir/build

symtab/CMakeFiles/lexLeo_symtab.dir/clean:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab && $(CMAKE_COMMAND) -P CMakeFiles/lexLeo_symtab.dir/cmake_clean.cmake
.PHONY : symtab/CMakeFiles/lexLeo_symtab.dir/clean

symtab/CMakeFiles/lexLeo_symtab.dir/depend:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/workaholic/env/clion/workspace/lexLeo /home/workaholic/env/clion/workspace/lexLeo/symtab /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/symtab/CMakeFiles/lexLeo_symtab.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : symtab/CMakeFiles/lexLeo_symtab.dir/depend

