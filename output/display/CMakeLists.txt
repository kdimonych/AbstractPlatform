cmake_minimum_required(VERSION 3.13)

project(abstract-platform.output.display CXX)

option(ENABLE_ASSERTS_FOR_DISPLAY
       "Enable asserts for abstract-platform.output.display" ON)

set(HEADER_LIST
    AbstractPlatform/output/display/AbstractDisplayDriver.hpp
    AbstractPlatform/output/display/buffer/PixelBufferImpl.hpp
    AbstractPlatform/output/display/buffer/StaticPixelBuffer.hpp
    AbstractPlatform/output/display/buffer/PixelBufferView.hpp
    AbstractPlatform/output/display/Drawers.hpp
    AbstractPlatform/output/display/Pixel.hpp
    AbstractPlatform/output/display/ColorProcessing.hpp
    AbstractPlatform/output/display/Position.hpp)

set(SOURCE_LIST)

# Add library cpp files
add_library(abstract-platform.output.display INTERFACE)

if(NOT ENABLE_ASSERTS_FOR_DISPLAY)
  target_compile_definitions(abstract-platform.output.display INTERFACE NDEBUG)
endif()

# Associate the sources with the library Note: The source files are not
# compiled, as this is an interface library.
target_sources(
  abstract-platform.output.display
  INTERFACE ${HEADER_LIST}
  PUBLIC ${SOURCE_LIST})

target_link_libraries(
  abstract-platform.output.display INTERFACE abstract-platform.common
                                             abstract-platform.platform)

# Add include directory
target_include_directories(abstract-platform.output.display
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)
add_subdirectory(png_display)
