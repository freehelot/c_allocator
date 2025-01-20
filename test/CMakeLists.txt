enable_testing()

# Add path to Unity source code
set(UNITY_DIR "${CMAKE_SOURCE_DIR}/test/unity")

# Add Unity source files to the test build
set(UNITY_SOURCES ${UNITY_DIR}/unity.c)

# Collect all test source files
file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.c")

# Create test executable/runner
add_executable(test_runner ${TEST_SOURCES} ${UNITY_SOURCES})

# Include directories for Unity
target_include_directories(test_runner PRIVATE ${UNITY_DIR} ${CMAKE_SOURCE_DIR}/inc)

# Link test executable with Unity Framework
target_link_libraries(test_runner PRIVATE MyCProject)

# Set output directory for test executables
set_target_properties(test_runner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")

# Add a test case for CTest
add_test(NAME test_runner COMMAND test_runner)
