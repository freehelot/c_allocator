# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project Name
project(MyCProject C)

# Standards
set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17) # unused
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Definitions to be used within source code
add_definitions(-DALLOC_BLOCK_SIZE=16 -DALLOC_NUM_BLOCKS=10)

# Define options to chose between release and unit tests
option(BUILD_UNIT_TESTS "Build the project with unit tests" OFF)
option(EMBEDDED_TARGET "Build and run project on embedded target" OFF)

# Build executable
add_subdirectory(src) 
# Add header files
include_directories(${CMAKE_SOURCE_DIR}/inc)

if(BUILD_UNIT_TESTS)
    # Unit tests
    enable_testing()
    add_subdirectory(test)
endif()
