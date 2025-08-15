cmake_minimum_required(VERSION 3.13)

# Ensure the build is for the same architecture as the host
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
  return()
endif()

# Find GoogleTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  message(WARNING "GTest not found, skipping tests in this directory.")
  return()
endif()

# Project definition
project(abstract-platform.common_test C CXX)

# Enable testing
enable_testing()

# Header files for the test executable. Currently we are not using any header
# files in the test but we are keeping this variable for future use and to keep
# the structure similar to the other test files
set(HEADER_LIST)
# Source files for the test executable
set(SOURCE_LIST
    BinaryOperationsTest.cpp
    MemoryTest.cpp
    BitOperationsTest.cpp
    BufferTest.cpp
    SimpleMathTest.cpp
    DuplicatorTest.cpp
    BufferTraitsTest.cpp
    BitBufferTest.cpp)

# Create the test executable
add_executable(abstract-platform.common_test ${SOURCE_LIST})

# Link libraries
target_link_libraries(abstract-platform.common_test abstract-platform.platform
                      abstract-platform.common GTest::gtest_main GTest::gmock)

# Discover GoogleTest tests
include(GoogleTest)
gtest_discover_tests(abstract-platform.common_test)
