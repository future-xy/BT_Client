# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/67/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/67/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/include.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/include.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/include.dir/flags.make

CMakeFiles/include.dir/hash.cpp.o: CMakeFiles/include.dir/flags.make
CMakeFiles/include.dir/hash.cpp.o: ../hash.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/include.dir/hash.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/include.dir/hash.cpp.o -c /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/hash.cpp

CMakeFiles/include.dir/hash.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/include.dir/hash.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/hash.cpp > CMakeFiles/include.dir/hash.cpp.i

CMakeFiles/include.dir/hash.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/include.dir/hash.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/hash.cpp -o CMakeFiles/include.dir/hash.cpp.s

# Object files for target include
include_OBJECTS = \
"CMakeFiles/include.dir/hash.cpp.o"

# External object files for target include
include_EXTERNAL_OBJECTS =

libinclude.a: CMakeFiles/include.dir/hash.cpp.o
libinclude.a: CMakeFiles/include.dir/build.make
libinclude.a: CMakeFiles/include.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libinclude.a"
	$(CMAKE_COMMAND) -P CMakeFiles/include.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/include.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/include.dir/build: libinclude.a

.PHONY : CMakeFiles/include.dir/build

CMakeFiles/include.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/include.dir/cmake_clean.cmake
.PHONY : CMakeFiles/include.dir/clean

CMakeFiles/include.dir/depend:
	cd /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug /home/jijorbq/Documents/allEXPR/BT_Client/BT_Client/cmake-build-debug/CMakeFiles/include.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/include.dir/depend
