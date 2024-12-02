cmake_minimum_required(VERSION 3.13)

project(abstract-platform.common C CXX)

set(HEADER_LIST
	AbstractPlatform/common/ErrorCode.hpp 
    AbstractPlatform/common/Platform.hpp 
    AbstractPlatform/common/PlatformLiteral.hpp
    AbstractPlatform/common/ArrayHelper.hpp
    AbstractPlatform/common/BinaryOperations.hpp
    )

set(SOURCE_LIST )

# Add library cpp files
add_library(abstract-platform.common INTERFACE ${HEADER_LIST} ${SOURCE_LIST}) 

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
    target_compile_definitions(abstract-platform.common INTERFACE __EXCEPTIONS)
else()
    target_compile_definitions(abstract-platform.common INTERFACE -fno-exceptions)
endif()

target_sources(abstract-platform.common
    INTERFACE ${HEADER_LIST}
    PUBLIC ${SOURCE_LIST}
)

# Add include directory
target_include_directories(abstract-platform.common INTERFACE ${CMAKE_CURRENT_LIST_DIR})