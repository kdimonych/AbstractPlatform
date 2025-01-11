cmake_minimum_required(VERSION 3.13)

project(abstract-platform.tensor CXX)

set(HEADER_LIST
    AbstractPlatform/tensor/StaticTensorIterator.hpp
    )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.tensor INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
    target_compile_definitions(abstract-platform.tensor INTERFACE __EXCEPTIONS)
else()
    target_compile_definitions(abstract-platform.tensor INTERFACE -fno-exceptions)
endif()

target_sources(abstract-platform.tensor
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

target_link_libraries(abstract-platform.tensor INTERFACE abstract-platform.common)

# Add include directory
target_include_directories(abstract-platform.tensor INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)