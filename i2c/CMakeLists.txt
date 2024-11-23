cmake_minimum_required(VERSION 3.13)

project(abstract-platform.i2c)

set(HEADER_LIST
    AbstractI2C.hpp )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.i2c INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
    target_compile_definitions(abstract-platform.i2c INTERFACE __EXCEPTIONS)
else()
    target_compile_definitions(abstract-platform.i2c INTERFACE -fno-exceptions)
endif()

target_sources(abstract-platform.i2c INTERFACE
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

target_link_libraries(abstract-platform.i2c INTERFACE abstract-platform.platform)

# Add include directory
target_include_directories(abstract-platform.i2c INTERFACE ${CMAKE_CURRENT_LIST_DIR})