cmake_minimum_required(VERSION 3.13)
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
    return()
endif()

project(abstract-platform.output.display_test C CXX)
find_package(GTest REQUIRED)

enable_testing()

set(HEADER_LIST )
set(SOURCE_LIST StaticCanvasViewTest.cpp)

add_executable(abstract-platform.output.display_test ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(abstract-platform.output.display_test abstract-platform.output.display abstract-platform.common GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(abstract-platform.output.display_test)

