cmake_minimum_required(VERSION 3.13)

# Ensure the build is for the same architecture as the host
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
  return()
endif()

# Project definition
project(abstract-platform.common_test C CXX)

# Enable testing
enable_testing()

# Find GoogleTest
find_package(GTest REQUIRED)

# Header files for the test executable Curently we are not using any header
# files in the test but we are keeping this variable for future use and to keep
# the structure similar to the other test files
set(HEADER_LIST)
# Source files for the test executable
set(SOURCE_LIST CanvasViewTest.cpp AbstractCanvasTest.cpp)

# Create the test executable
add_executable(abstract-platform.output.display_test ${HEADER_LIST}
                                                     ${SOURCE_LIST})
# Link libraries
target_link_libraries(
  abstract-platform.output.display_test abstract-platform.output.display
  abstract-platform.common GTest::gtest_main)

# Set the C++ standard explicitly
target_compile_features(abstract-platform.output.display_test
                        PRIVATE cxx_std_17)

# Discover GoogleTest tests
include(GoogleTest)
gtest_discover_tests(abstract-platform.output.display_test)
