cmake_minimum_required(VERSION 3.13)
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
    return()
endif()

enable_testing()

project(abstract-platform.common_test C CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(GTest REQUIRED)

set(HEADER_LIST )
set(SOURCE_LIST 
    TensorIteratorTest.cpp 
    MemoryTest.cpp
    )

include(GoogleTest)

add_executable(abstract-platform.common_test ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(abstract-platform.common_test abstract-platform.common GTest::gtest_main)

gtest_add_tests(abstract-platform.common_test "" AUTO)
gtest_discover_tests(abstract-platform.common_test)