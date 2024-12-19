cmake_minimum_required(VERSION 3.13)

project(abstract-platform.output.display CXX)

set(HEADER_LIST
	AbstractPlatform/output/display/AbstractDisplay.hpp 
    AbstractPlatform/output/display/Drawer.hpp
    AbstractPlatform/output/display/StaticCanvasView.hpp
    )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.output.display INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

target_sources(abstract-platform.output.display
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

target_link_libraries(abstract-platform.output.display INTERFACE abstract-platform.common abstract-platform.tensor)

# Add include directory
target_include_directories(abstract-platform.output.display INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)