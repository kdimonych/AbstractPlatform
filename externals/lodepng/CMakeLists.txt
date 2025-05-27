cmake_minimum_required(VERSION 3.13)

# Property to enable lodepng library support
set(ENABLE_LODEPNG
    ON
    CACHE BOOL "Enable lodepng library support")

# Ensure the build is for the same architecture as the host
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR}
   OR NOT ${ENABLE_LODEPNG})
  return()
endif()

include(FetchContent)

set(LODEPNG_TAG
    master
    CACHE STRING "Branch or tag for externals.lodepng repository")

FetchContent_Declare(
  externals.lodepng
  GIT_REPOSITORY https://github.com/lvandeve/lodepng.git
  GIT_TAG ${LODEPNG_TAG})
FetchContent_MakeAvailable(externals.lodepng)

FetchContent_GetProperties(externals.lodepng SOURCE_DIR LODEPNG_SOURCE_DIR)

message(STATUS "externals.lodepng source directory: " "${LODEPNG_SOURCE_DIR}")

# Add the include directory manually
set(LODEPNG_HEADERS ${LODEPNG_SOURCE_DIR}/lodepng.h)
set(LODEPNG_SOURCES ${LODEPNG_SOURCE_DIR}/lodepng.cpp)
add_library(externals.lodepng ${LODEPNG_HEADERS} ${LODEPNG_SOURCES})
target_include_directories(externals.lodepng INTERFACE ${LODEPNG_SOURCE_DIR})
