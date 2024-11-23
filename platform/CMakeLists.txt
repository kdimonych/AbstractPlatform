cmake_minimum_required(VERSION 3.13)

project(abstract-platform.platform)

set(HEADER_LIST
    Platform.hpp PlatformLiteral.hpp )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.platform INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
    target_compile_definitions(abstract-platform.platform INTERFACE __EXCEPTIONS)
else()
    target_compile_definitions(abstract-platform.platform INTERFACE -fno-exceptions)
endif()

target_sources(abstract-platform.platform INTERFACE
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

# Add include directory
target_include_directories(abstract-platform.platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})