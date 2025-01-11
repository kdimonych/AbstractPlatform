cmake_minimum_required(VERSION 3.13)

project(abstract-platform.output.display CXX)

set(HEADER_LIST
	AbstractPlatform/output/display/AbstractDisplay.hpp 
    AbstractPlatform/output/display/Drawer.hpp
    )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.output.display INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
    target_compile_definitions(abstract-platform.output.display INTERFACE __EXCEPTIONS)
else()
    target_compile_definitions(abstract-platform.output.display INTERFACE -fno-exceptions)
endif()

target_sources(abstract-platform.output.display
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

# Add include directory
target_include_directories(abstract-platform.output.display INTERFACE ${CMAKE_CURRENT_LIST_DIR})