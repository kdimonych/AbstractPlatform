cmake_minimum_required(VERSION 3.13)
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
    return()
endif()

project(abstract-platform.tensor_test C CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
find_package(GTest REQUIRED)

set(HEADER_LIST )
set(SOURCE_LIST 
    StaticTensorIteratorTest.cpp
    )

include(GoogleTest)

add_executable(abstract-platform.tensor_test ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(abstract-platform.tensor_test abstract-platform.tensor GTest::gtest_main)

gtest_add_tests(abstract-platform.tensor_test "" AUTO)
gtest_discover_tests(abstract-platform.tensor_test)