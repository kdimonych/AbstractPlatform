cmake_minimum_required(VERSION 3.13)

project(abstract-platform C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(common)
add_subdirectory(i2c)
add_subdirectory(output)
add_subdirectory(tensor)
