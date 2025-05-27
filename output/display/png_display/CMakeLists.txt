cmake_minimum_required(VERSION 3.13)

# Ensure the build is for the same architecture as the host
if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR}
   OR NOT ${ENABLE_LODEPNG})
  return()
endif()

project(abstract-platform.output.display.png_display CXX)

set(HEADER_LIST
    AbstractPlatform/output/display/png_display/PngDisplayDriver.hpp)

set(SOURCE_LIST
    AbstractPlatform/output/display/png_display/PngDisplayDriver.cpp)

# Add library
add_library(abstract-platform.output.display.png_display)
# Associate the sources with the library. Note: The source files are not
# compiled, as this is an interface library.
target_sources(abstract-platform.output.display.png_display
               PUBLIC ${HEADER_LIST} ${SOURCE_LIST})

target_link_libraries(
  abstract-platform.output.display.png_display
  PUBLIC externals.lodepng abstract-platform.common
         abstract-platform.output.display)

# Add include directory
target_include_directories(abstract-platform.output.display.png_display
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)
