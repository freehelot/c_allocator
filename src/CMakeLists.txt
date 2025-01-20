# CMakeLists.txt src
# Build block allocator as library
add_library(MyCProject STATIC ${CMAKE_SOURCE_DIR}/src/block.c) 

# Include directories
target_include_directories(MyCProject PRIVATE ${CMAKE_SOURCE_DIR}/inc)